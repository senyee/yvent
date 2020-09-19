#include <unistd.h>
#include <cassert>
#include <sys/sendfile.h>
#include <limits.h>
#include "TcpConnection.h"
#include "Logging.h"
#include "EventLoop.h"
#include "Channel.h"

using namespace yvent;

TcpConnection::TcpConnection(EventLoop *loop, const std::string &name,
                            int cfd, const InetAddr &local, const InetAddr &peer)
        :loop_(loop),
        name_(name),
        cfd_(cfd),
        local_(local),
        peer_(peer),
        channel_(loop, cfd),
        state_(kConnecting),
        sendFd_(-1)
{
    channel_.setReadCallback([this](){this->handleRead();});
    channel_.setCloseCallback([this](){this->handleClose();});
    channel_.setErrorCallback([this](){this->handleError();});
    channel_.setWriteCallback([this](){this->handleWrite();});
    memset(&sbuf_, 0, sizeof(sbuf_));
}

TcpConnection::~TcpConnection()
{
    LOG_TRACE("TcpConnection die");
    assert(state_ == kDisconnected);
    ::close(cfd_);
}

void TcpConnection::enableRead()
{
    assert(state_ == kConnecting);
    state_ = kConnected;
    channel_.enableRead();
    if (connectionCallback_) connectionCallback_(shared_from_this());
}

void TcpConnection::handleRead()
{
    int savedErrno;
    ssize_t n = inBuffer_.readFd(cfd_, &savedErrno);
    if(-1 == n) {
        errno = savedErrno;
        LOG_SYSERR("errno = %d", savedErrno);
    } else if(0 == n) {
        handleClose();
    } else if(messageCallback_) {
        messageCallback_(shared_from_this(), &inBuffer_);
    }
}

void TcpConnection::handleClose()
{
    assert(loop_->isInLoopThread());
    assert(state_ == kConnected ||
           state_ == kDisconnecting);
    state_ = kDisconnected;
    LOG_TRACE("closed:%s", name_.c_str());
    channel_.disableAll();
    if (connectionCallback_) connectionCallback_(shared_from_this());
    if (closeCallback_) closeCallback_(shared_from_this());
}

void TcpConnection::handleError()
{
    int err;
    socklen_t len = sizeof(err);
    int ret = getsockopt(cfd_, SOL_SOCKET, SO_ERROR, &err, &len);
    if (ret != -1)
        errno = err;
    LOG_SYSERR("err = %d", err);
}

void TcpConnection::send(Buffer& buffer)
{
    if (state_ != kConnected) {
        LOG_WARN("TcpConnection not connected,, give up send");
        return;
    }
    if (loop_->isInLoopThread()) {
        sendInLoop(buffer.peek(), buffer.readableBytes());
        buffer.retrieveAll();
    }
    else {
        loop_->runInLoop([ptr = shared_from_this(), 
                          str = buffer.retrieveAllAsString()]()
                {
                    ptr->sendInLoop(str); 
                });
    }
}

void TcpConnection::send(const std::string& message)
{
    send(message.data(), message.size());
}

void TcpConnection::send(const char* data, size_t len)
{
    if (state_ != kConnected) {
        LOG_WARN("TcpConnection not connected,, give up send");
        return;
    }
    if (loop_->isInLoopThread()) {
        sendInLoop(data, len);
    } else {
        loop_->runInLoop([ptr = shared_from_this(),str=std::string(data,len)](){
            ptr->sendInLoop(str);
        });
    }
}

void TcpConnection::sendInLoop(const std::string& message)
{
    sendInLoop(message.data(), message.size());
}

void TcpConnection::sendInLoop(const char* data, size_t len)
{
    assert(loop_->isInLoopThread());
    if (state_ == kDisconnected) {
        LOG_WARN("TcpConnection disconnected, give up send");
        return;
    }
    ssize_t n = 0;
    //try writing directly
    if (!channel_.isWriting() && outBuffer_.readableBytes() == 0) {
        n = ::write(channel_.fd(), data, len);
        if(n == -1) {
            if (errno != EAGAIN) {
                LOG_SYSERR("errno = %d",errno);
            }
            n = 0;
        } else if (static_cast<size_t>(n) < len) {
            LOG_TRACE("remain data to send");
        } else if (writeCompleteCallback_) {
            loop_->runInLoop([ptr = shared_from_this()](){
                ptr->writeCompleteCallback_(ptr);
            });
        }
    }

    assert(n >= 0);
    if (static_cast<size_t>(n) < len || sbuf_.st_size != 0) {
        outBuffer_.append(data + n, len - n);
        if (!channel_.isWriting()) {
            channel_.enableWrite();
        }
    }
}

void TcpConnection::handleWrite()
{
    assert(loop_->isInLoopThread());
    if (channel_.isWriting()) {
        if(outBuffer_.readableBytes() != 0) {
            ssize_t n = ::write(channel_.fd(), outBuffer_.peek(), outBuffer_.readableBytes());
            if (n == -1) {
                LOG_SYSERR("::write");
            } else {
                outBuffer_.retrieve(static_cast<size_t>(n));
                if (outBuffer_.readableBytes() == 0 && sbuf_.st_size == 0) {
                    channel_.disableWrite();
                    if (state_ == kDisconnecting) {
                        shutdownInLoop();
                    }
                    if (writeCompleteCallback_) {
                        loop_->runInLoop([ptr = shared_from_this()](){
                            ptr->writeCompleteCallback_(ptr);
                        });
                    }

                } else {
                    LOG_TRACE("more data to write");
                }
            }
        } else if (sbuf_.st_size != 0) {
            ssize_t n = ::sendfile(channel_.fd(), sendFd_, NULL, INT_MAX);
            if(n < 0) return;
            sbuf_.st_size -= n;
            if(sbuf_.st_size == 0)
            {
                channel_.disableWrite();
            }
        }
    }
    else {
        LOG_TRACE("TcpConnection is down, no more writing");
    }
}

void TcpConnection::shutdown()
{
    if (state_ == kConnected) {
        state_ = kDisconnecting;
        loop_->runInLoop([ptr = shared_from_this()](){
            ptr->shutdownInLoop();
        });
    }
}

void TcpConnection::shutdownInLoop()
{
    assert(loop_->isInLoopThread());
    if (!channel_.isWriting()) {
        if (::shutdown(channel_.fd(), SHUT_WR) < 0) {
            LOG_SYSERR("::shutdown err");
        }
    }
}

void TcpConnection::forceClose()
{
    if (state_ != kDisconnected) {
        loop_->runInLoop( std::bind(&TcpConnection::forceCloseInLoop, shared_from_this()) );
    }
}

void TcpConnection::forceCloseInLoop()
{
    assert(loop_->isInLoopThread());
    if (state_ != kDisconnected) {
        handleClose();
    }
}

bool TcpConnection::connected() const
{
    return state_ == kConnected;
}

bool TcpConnection::disconnected() const
{
    return state_ == kDisconnected;
}






