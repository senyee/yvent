#include <strings.h>
#include <arpa/inet.h>
#include "Logging.h"
#include "InetAddr.h"

using namespace yvent;

InetAddr::InetAddr(uint16_t port, bool loopback /*=false*/)
{
    bzero(&addr_, sizeof(addr_));
    addr_.sin_family = AF_INET;
    //in_addr_t ip = loopback ? INADDR_LOOPBACK : INADDR_ANY;
    //addr_.sin_addr.s_addr = htonl(ip);
    addr_.sin_addr.s_addr = INADDR_ANY;
    addr_.sin_port = htons(port);
}

InetAddr::InetAddr(const std::string& ip, uint16_t port)
{
    bzero(&addr_, sizeof(addr_));
    addr_.sin_family = AF_INET;
    int ret = ::inet_pton(AF_INET, ip.c_str(), &addr_.sin_addr.s_addr);
    if (ret != 1)
        LOG_SYSFATAL("InetAddress::inet_pton()\n");

    addr_.sin_port = htons(port);
}

InetAddr::~InetAddr()
{
    
}

