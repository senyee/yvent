#ifndef YVENT_TCPCONNECTION_H
#define YVENT_TCPCONNECTION_H
#include "Callbacks.h"
#include "InetAddr.h"
#include "Channel.h"
namespace yvent
{

class EventLoop;
class TcpConnection
{
public:
    TcpConnection(EventLoop *loop, int cfd, const InetAddr &local, const InetAddr &peer);
    ~TcpConnection();

    void enableRead();
    void setReadCallback(const MessageCallback &cb)
        {messageCallback_ = cb;}
private:
    void handleRead();
private:
    EventLoop *loop_;
    int cfd_;
    InetAddr local_;
    InetAddr peer_;
    Channel channel_;
    MessageCallback messageCallback_;
};

} // namespace yvent



#endif //YVENT_TCPCONNECTION_H