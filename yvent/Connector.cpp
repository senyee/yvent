#include <cassert>
#include <unistd.h>//close
#include "Connector.h"
#include "Socket.h"
#include "Logging.h"
#include "EventLoop.h"
#include "InetAddr.h"
using namespace yvent;

Connector::Connector(EventLoop* loop, const InetAddr& server)
        :loop_(loop),
        server_(server),
        cfd_(sockets::Socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0)),
        connected_(false),
        started_(false),
        channel_(loop, cfd_)
{
    channel_.setWriteCallback([this](){
        this->handleWrite();
    });
}

Connector::~Connector()
{
    if (!connected_)
        ::close(cfd_);
}

void Connector::start()
{
    assert(loop_->isInLoopThread());
    assert(!started_);
    started_ = true;

    int ret = ::connect(cfd_, server_.getSockaddr(), server_.getSockaddrLen());
    if (ret == -1) {
        LOG_TRACE("connect ret errno= %d", errno);
        if (errno != EINPROGRESS) {
            handleWrite();
        } else {
            channel_.enableWrite();
        }
    } else {
        handleWrite();
    }
}

void Connector::handleWrite()
{
    assert(loop_->isInLoopThread());
    assert(started_);

    channel_.disableAll();

    int err;
    socklen_t len = sizeof(err);
    int ret = ::getsockopt(cfd_, SOL_SOCKET, SO_ERROR, &err, &len);
    if (ret == 0)
        errno = err;
    if (errno != 0) {
        LOG_SYSERR("::connect errno = %d", errno);
        if (errorCallback_) {
            errorCallback_();
        }
    } else if (newConnectionCallback_) {
        struct sockaddr_in addr;
        len = sizeof(addr);
        ret = ::getsockname(cfd_, reinterpret_cast<sockaddr*>(&addr), &len);
        if (ret == -1)
            LOG_SYSERR("getsockname");
        InetAddr local(0);
        local.setAddr(addr);

        // now cfd_ is not belong to us
        connected_ = true;
        newConnectionCallback_(cfd_, local);
    }
}