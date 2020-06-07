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
        channel_(loop, cfd)
{
    channel_.setReadCallback([this](){this->handleRead();});
    channel_.setCloseCallback([this](){this->handleClose();});
    channel_.setErrorCallback([this](){this->handleError();});
}

TcpConnection::~TcpConnection()
{
    ::close(cfd_);
}

void TcpConnection::enableRead()
{
    channel_.enableRead();
}

void TcpConnection::handleRead()
{
    char buf[BUFSIZ] = {0};
    ssize_t n = ::read(cfd_, buf, BUFSIZ);
    if(-1 == n) {

    } else if(0 == n) {
        handleClose();
    } else if(messageCallback_) {
        messageCallback_(buf, BUFSIZ);
    }
}

void TcpConnection::handleClose()
{
    assert(loop_->isInLoopThread());
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

