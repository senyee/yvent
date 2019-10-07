#ifndef YVENT_INETADDR_H
#define YVENT_INETADDR_H

#include <netinet/in.h>
#include <string>

namespace yvent
{

class InetAddr
{
public:
    explicit
    InetAddr(uint16_t port, bool loopback = false);
    InetAddr(const std::string& ip, uint16_t port);
    ~InetAddr();

    const struct sockaddr * getSockaddr() const 
        {return reinterpret_cast<const struct sockaddr *>(&addr_);}
    socklen_t getSockaddrLen() const
        {return sizeof(addr_);}
    void setAddr(const struct sockaddr_in &addr);
private:
    struct sockaddr_in addr_;

};

} // namespace yvent


#endif //YVENT_INETADDR_H