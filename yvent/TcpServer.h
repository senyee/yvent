#ifndef YVENT_TCPSERVER_H
#define YVENT_TCPSERVER_H
#include <memory>
#include <map>
#include "InetAddr.h"
#include "Acceptor.h"
#include "Callbacks.h"
#include "TcpConnection.h"

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
private:
    void newConnection(int cfd, const InetAddr &peer);
    void handleClose(const TcpConnectionPtr&);

private:
    typedef std::map<std::string,TcpConnectionSptr> ConnectionMap;
    EventLoop *loop_;
    InetAddr host_;
    int nextConnId_;
    Acceptor acceptor_;
    MessageCallback messageCallback_;
    ConnectionMap connections_;
};


    
} // namespace yvent

#endif //YVENT_TCPSERVER_H