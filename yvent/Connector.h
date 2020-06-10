#ifndef YVENT_CONNECTOR_H
#define YVENT_CONNECTOR_H

#include <functional>

#include "InetAddr.h"
#include "Channel.h"
#include "noncopyable.h"
#include "Callbacks.h"

namespace yvent
{

class EventLoop;
class InetAddr;

class Connector: public noncopyable
{
public:
    Connector(EventLoop* loop, const InetAddr& server);
    ~Connector();

    void start();

    void setNewConnectionCallback(const NewConnectionCallback& cb)
        { newConnectionCallback_ = cb; }

    void setErrorCallback(const ErrorCallback& cb)
        { errorCallback_ = cb; }

private:
    void handleWrite();

    EventLoop* loop_;
    const InetAddr server_;
    const int cfd_;
    bool connected_;
    bool started_;
    Channel channel_;
    NewConnectionCallback newConnectionCallback_;
    ErrorCallback errorCallback_;
};

}


#endif //YVENT_CONNECTOR_H
