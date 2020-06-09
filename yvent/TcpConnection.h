#ifndef YVENT_TCPCONNECTION_H
#define YVENT_TCPCONNECTION_H
#include <functional>
#include <memory>
#include <atomic>
#include "Callbacks.h"
#include "InetAddr.h"
#include "Channel.h"
#include "TcpConnection.h"
#include "Buffer.h"
namespace yvent
{

enum ConnectionState
{
    kConnecting,
    kConnected,
    kDisconnecting,
    kDisconnected
};


class EventLoop;
class TcpConnection:public noncopyable,
                    public std::enable_shared_from_this<TcpConnection>
{
public:
    TcpConnection(EventLoop *loop, const std::string &name, int cfd, const InetAddr &local, const InetAddr &peer);
    ~TcpConnection();

    void enableRead();
    void setReadCallback(const MessageCallback &cb)
        { messageCallback_ = cb; }
    void setCloseCallback(const CloseCallback &cb)
        { closeCallback_ = cb; }
    void setWriteCompleteCallback(const WriteCompleteCallback& cb)
        { writeCompleteCallback_ = cb; }
    std::string name() const
        {return name_;}
    void send(const std::string& message);
    void shutdown();
private:
    void handleRead();
    void handleClose();
    void handleError();
    void handleWrite();
    void sendInLoop(const std::string& message);
    void shutdownInLoop();
private:
    EventLoop *loop_;
    std::string name_;
    int cfd_;
    InetAddr local_;
    InetAddr peer_;
    Channel channel_;
    MessageCallback messageCallback_;
    CloseCallback   closeCallback_;
    WriteCompleteCallback writeCompleteCallback_;
    Buffer inBuffer_;
    Buffer outBuffer_;
    std::atomic_int32_t state_;
};

} // namespace yvent


#endif //YVENT_TCPCONNECTION_H