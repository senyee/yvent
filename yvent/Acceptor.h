#ifndef YVENT_ACCEPTOR_H
#define YVENT_ACCEPTOR_H
#include <functional>
#include "noncopyable.h"
#include "InetAddr.h"
#include "Channel.h"
#include "Callbacks.h"
namespace yvent
{

class EventLoop;
class InetAddr;
class Acceptor:public noncopyable
{
public:
    Acceptor(EventLoop* loop, const InetAddr& host);
    ~Acceptor();

    void listen();
    void setNewConnectionCallback(const NewConnectionCallback &cb)
        { newConnectionCallback_ = cb;}
private:
    void handleRead();
private:
    EventLoop* loop_;
    int lfd_;
    InetAddr host_;
    Channel acceptChannel_;
    NewConnectionCallback newConnectionCallback_;
};

} // namespace yvent


#endif //YVENT_ACCEPTOR_H