#include <cassert>
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

}

void Acceptor::listen()
{
    LOG_TRACE("listen...\n");
    assert(loop_->isInLoopThread());
    sockets::Listen(lfd_,SOMAXCONN);
    acceptChannel_.enableRead();
}