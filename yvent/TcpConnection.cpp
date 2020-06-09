#include <unistd.h>
#include <cassert>
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
        state_(kConnecting)
{
    channel_.setReadCallback([this](){this->handleRead();});
    channel_.setCloseCallback([this](){this->handleClose();});
    channel_.setErrorCallback([this](){this->handleError();});
    channel_.setWriteCallback([this](){this->handleWrite();});
}

TcpConnection::~TcpConnection()
{
    assert(state_ == kDisconnected);
    ::close(cfd_);
}

void TcpConnection::enableRead()
{
    assert(state_ == kConnecting);
    state_ = kConnected;
    channel_.enableRead();
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
    if(closeCallback_) closeCallback_(shared_from_this());
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

void TcpConnection::send(const std::string& message)
{
    if (state_ != kConnected) {
        LOG_WARN("TcpConnection not connected,, give up send");
        return;
    }
    if (loop_->isInLoopThread()) {
        sendInLoop(message);
    } else {
        loop_->runInLoop([&](){
            this->sendInLoop(message);
        });
    }
}

void TcpConnection::sendInLoop(const std::string& message)
{
    assert(loop_->isInLoopThread());
    if (state_ == kDisconnected) {
        LOG_WARN("TcpConnection disconnected, give up send");
        return;
    }
    ssize_t n = 0;
    //try writing directly
    if (!channel_.isWriting() && outBuffer_.readableBytes() == 0) {
        n = ::write(channel_.fd(), message.data(), message.size());
        if(n == -1) {
            if (errno != EAGAIN) {
                LOG_SYSERR("errno = %d",errno);
            }
            n = 0;
        } else if (static_cast<size_t>(n) < message.size()) {
            LOG_TRACE("remain data to send");
        } else if (writeCompleteCallback_) {
            loop_->runInLoop([this](){
                this->writeCompleteCallback_(shared_from_this());
            });
        }
    }

    assert(n >= 0);
    if (static_cast<size_t>(n) < message.size()) {
        outBuffer_.append(message.data() + n, message.size() - n);
        if (!channel_.isWriting()) {
            channel_.enableWrite();
        }
    }
}

void TcpConnection::handleWrite()
{
    assert(loop_->isInLoopThread());
    if (channel_.isWriting()) {
        ssize_t n = ::write(channel_.fd(), outBuffer_.peek(), outBuffer_.readableBytes());
        if (n == -1) {
            LOG_SYSERR("::write");
        } else {
            outBuffer_.retrieve(static_cast<size_t>(n));
            if (outBuffer_.readableBytes() == 0) {
                channel_.disableWrite();
                if (state_ == kDisconnecting) {
                    shutdownInLoop();
                }
                if (writeCompleteCallback_) {
                    loop_->runInLoop([this](){
                        this->writeCompleteCallback_(shared_from_this());
                    });
                }
            } else {
                LOG_TRACE("more data to write");
            }
        }
    } else {
        LOG_TRACE("TcpConnection is down, no more writing");
    }
}

void TcpConnection::shutdown()
{
    if (state_ == kConnected) {
        state_ = kDisconnecting;
        loop_->runInLoop([this](){
            shared_from_this()->shutdownInLoop();
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






