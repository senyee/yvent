#include "Socket.h"
#include "Logging.h"
using namespace yvent;

int sockets::Socket (int domain, int type, int protocol)
{
    int ret = ::socket(domain, type, protocol);
    if (ret == -1)
        LOG_SYSFATAL("Acceptor::socket() err");
    return ret;
}

int sockets::Bind (int fd, const struct sockaddr *addr, socklen_t len)
{
    int ret = ::bind(fd, addr, len);
    if (ret == -1)
        LOG_SYSFATAL("Acceptor::socket() err");
    return ret;
}

int sockets::Connect (int fd, const struct sockaddr *addr, socklen_t len)
{
    int ret = ::connect(fd, addr, len);
    if (ret == -1)
        LOG_SYSFATAL("Acceptor::socket() err");
    return ret;
}

int sockets::Accept (int fd, struct sockaddr *addr, socklen_t *len)
{
    int ret = ::accept(fd, addr, len);
    if (ret == -1)
        LOG_SYSFATAL("Acceptor::socket() err");
    return ret;
}

int sockets::Listen (int fd, int n)
{
    int ret = ::listen(fd,n);
    if (ret == -1)
        LOG_SYSFATAL("Acceptor::socket() err");
    return ret;
}

int sockets::Setsockopt(int sockfd, int level, int optname,
                const void *optval, socklen_t optlen)
{
    int ret = ::setsockopt(sockfd, level, optname, optval, optlen);
    if (ret == -1)
        LOG_SYSFATAL("Acceptor::socket() err");
    return ret;
}



