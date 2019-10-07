#include <cassert>
#include <unistd.h>
#include "Logging.h"
#include "Acceptor.h"
#include "Socket.h"
#include "EventLoop.h"
using namespace yvent;

Acceptor::Acceptor(EventLoop* loop, const InetAddr& host)
        :loop_(loop),
        lfd_( sockets::Socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0) ),
        host_(host),
        acceptChannel_(loop_, lfd_)
{
    assert(lfd_ == acceptChannel_.fd());
    int on = 1;
    sockets::Setsockopt(lfd_, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    sockets::Setsockopt(lfd_, SOL_SOCKET, SO_REUSEPORT, &on, sizeof(on));
    sockets::Bind(lfd_, host_.getSockaddr(), host_.getSockaddrLen());
}

Acceptor::~Acceptor()
{
    ::close(lfd_);
}

void Acceptor::listen()
{
    LOG_TRACE("listen...");
    assert(loop_->isInLoopThread());
    sockets::Listen(lfd_,SOMAXCONN);
    acceptChannel_.setReadCallback(std::bind(&Acceptor::handleRead,this));
    acceptChannel_.enableRead();
}

void Acceptor::handleRead()
{
    LOG_TRACE("new connection");
    assert(loop_->isInLoopThread());
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);

    void *any = &addr;
    int cfd = ::accept4(lfd_, static_cast<sockaddr*>(any),
                           &len, SOCK_NONBLOCK | SOCK_CLOEXEC);
    if (cfd == -1) {
        int savedErrno = errno;
        LOG_SYSERR("accept4()");
        switch (savedErrno) {
            case ECONNABORTED:
            case EMFILE:
                break;
            default:
                LOG_FATAL("unexpected accept4() error");
        }
    } else {
        InetAddr inetAddrPeer(0);
        inetAddrPeer.setAddr(addr);
        if(newConnectionCallback_) {
            newConnectionCallback_(cfd, inetAddrPeer);
        } else {
            ::close(cfd);
        }
    }
}