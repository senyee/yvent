#ifndef YVENT_ACCEPTOR_H
#define YVENT_ACCEPTOR_H
#include "noncopyable.h"
#include "InetAddr.h"
#include "Channel.h"
namespace yvent
{

class EventLoop;
class Acceptor:public noncopyable
{
public:
    Acceptor(EventLoop* loop, const InetAddr& host);
    ~Acceptor();

    void listen();
private:
    EventLoop* loop_;
    int lfd_;
    InetAddr host_;
    Channel acceptChannel_;
};

} // namespace yvent


#endif //YVENT_ACCEPTOR_H