#ifndef YVENT_TCPCONNECTION_H
#define YVENT_TCPCONNECTION_H
#include <functional>
#include <memory>
#include "Callbacks.h"
#include "InetAddr.h"
#include "Channel.h"
#include "TcpConnection.h"
#include "Buffer.h"
namespace yvent
{

class EventLoop;
class TcpConnection:public noncopyable,
                    public std::enable_shared_from_this<TcpConnection>
{
public:
    TcpConnection(EventLoop *loop, const std::string &name, int cfd, const InetAddr &local, const InetAddr &peer);
    ~TcpConnection();

    void enableRead();
    void setReadCallback(const MessageCallback &cb)
        {messageCallback_ = cb;}
    void setCloseCallback(const CloseCallback &cb)
        {closeCallback_ = cb;}
    std::string name() const
        {return name_;}
    void closeConnection();
private:
    void handleRead();
    void handleClose();
    void handleError();
private:
    EventLoop *loop_;
    std::string name_;
    int cfd_;
    InetAddr local_;
    InetAddr peer_;
    Channel channel_;
    MessageCallback messageCallback_;
    CloseCallback   closeCallback_;
    Buffer inBuffer_;
};

} // namespace yvent


#endif //YVENT_TCPCONNECTION_H