#include <unistd.h>
#include "TcpConnection.h"
using namespace yvent;

TcpConnection::TcpConnection(EventLoop *loop, int cfd, const InetAddr &local, const InetAddr &peer)
        :loop_(loop),
        cfd_(cfd),
        local_(local),
        peer_(peer),
        channel_(loop, cfd)
{
    channel_.setReadCallback([this](){this->handleRead();});
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
    ::read(cfd_, buf, BUFSIZ);
    if(messageCallback_) {
        messageCallback_(buf, BUFSIZ);
    }
}

