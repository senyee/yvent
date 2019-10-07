#ifndef YVENT_SOCKET_H
#define YVENT_SOCKET_H
#include <sys/socket.h>

namespace yvent
{
namespace sockets
{
    int Socket(int domain, int type, int protocol);
    int Bind (int fd, const struct sockaddr *addr, socklen_t len);
    int Connect (int fd, const struct sockaddr *addr, socklen_t len);
    int Accept (int fd, struct sockaddr *addr, socklen_t *len);
    int Listen (int fd, int n);
    int Setsockopt(int sockfd, int level, int optname,
                      const void *optval, socklen_t optlen);
} // namespace socket

} // namespace yvent


#endif //YVENT_SOCKET_H