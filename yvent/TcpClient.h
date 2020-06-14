#ifndef YVENT_TCPCLIENT_H
#define YVENT_TCPCLIENT_H

#include "Connector.h"
#include "Callbacks.h"
namespace yvent
{

class EventLoop;
class TcpClient
{
public:
    TcpClient(EventLoop* loop, const InetAddr server);
    ~TcpClient();

    void start();
    void setConnectionCallback(const ConnectionCallback& cb)
        { connectionCallback_ = cb; }
    void setMessageCallback(const MessageCallback& cb)
        { messageCallback_ = cb; }
    void setWriteCompleteCallback(const WriteCompleteCallback& cb)
        { writeCompleteCallback_ = cb; }
    void setErrorCallback(const ErrorCallback& cb)
        { connector_->setErrorCallback(cb); }
private:
    void newConnection(const int cfd, const InetAddr& local);
    void closeConnection(const TcpConnectionPtr& conn);

private:
    EventLoop* loop_;
    InetAddr   server_;
    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
    WriteCompleteCallback writeCompleteCallback_;
    std::unique_ptr<Connector> connector_;
    TcpConnectionPtr connection_;
};




}//yvent

#endif //YVENT_TCPCLIENT_H