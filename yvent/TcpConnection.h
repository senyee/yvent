#ifndef YVENT_TCPCONNECTION_H
#define YVENT_TCPCONNECTION_H
#include <functional>
#include <memory>
#include <atomic>
#include <any>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
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
    void setConnectionCallback(const ConnectionCallback& cb)
        { connectionCallback_ = cb; }
    void setReadCallback(const MessageCallback &cb)
        { messageCallback_ = cb; }
    void setCloseCallback(const CloseCallback &cb)
        { closeCallback_ = cb; }
    void setWriteCompleteCallback(const WriteCompleteCallback& cb)
        { writeCompleteCallback_ = cb; }
    std::string name() const
        {return name_;}
    void setContext(const std::any& context)
        { context_ = context; }
    const std::any& getContext() const
        { return context_; }
    std::any* getMutableContext()
        { return &context_; }
    void setSendFd(const int fd, const struct stat& sbuf)
        { sendFd_ = fd; 
            sbuf_ = sbuf;
        }
    void send(Buffer& buffer);
    void send(const std::string& message);
    void send(const char* data, size_t len);
    void shutdown();
    EventLoop* getLoop() const { return loop_; }
    void forceClose();
    bool connected() const;
    bool disconnected() const;
private:
    void handleRead();
    void handleClose();
    void handleError();
    void handleWrite();
    void sendInLoop(const std::string& message);
    void sendInLoop(const char* data, size_t len);
    void shutdownInLoop();
    void forceCloseInLoop();
private:
    EventLoop *loop_;
    std::string name_;
    int cfd_;
    InetAddr local_;
    InetAddr peer_;
    Channel channel_;
    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
    CloseCallback   closeCallback_;
    WriteCompleteCallback writeCompleteCallback_;
    Buffer inBuffer_;
    Buffer outBuffer_;
    std::atomic_int32_t state_;
    std::any context_;
    int sendFd_;
    struct stat sbuf_;
};

} // namespace yvent


#endif //YVENT_TCPCONNECTION_H