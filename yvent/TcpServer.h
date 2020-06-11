#ifndef YVENT_TCPSERVER_H
#define YVENT_TCPSERVER_H
#include <memory>
#include <map>
#include "InetAddr.h"
#include "Acceptor.h"
#include "Callbacks.h"
#include "TcpConnection.h"
#include "EventLoopThreadPool.h"

namespace yvent
{

class EventLoop;
class TcpServer:public noncopyable
{
public:
    typedef std::shared_ptr<TcpConnection> TcpConnectionSptr;
    TcpServer(EventLoop *loop, const InetAddr &host);
    ~TcpServer();

    void start();
    void setMessageCallback(const MessageCallback &cb)
        {messageCallback_ = cb;}
    void setThreadNum(int num) { threads_.setThreadNum(num); }
private:
    void newConnection(int cfd, const InetAddr &peer);
    void handleClose(const TcpConnectionPtr&);
    void handleCloseInLoop(const TcpConnectionPtr &tcpConnectionPtr);

private:
    typedef std::map<std::string,TcpConnectionSptr> ConnectionMap;
    EventLoop *loop_;
    InetAddr host_;
    int nextConnId_;
    Acceptor acceptor_;
    MessageCallback messageCallback_;
    ConnectionMap connections_;
    EventLoopThreadPool threads_;
};


    
} // namespace yvent

#endif //YVENT_TCPSERVER_H