#ifndef YVENT_EVENTLOOP_H
#define YVENT_EVENTLOOP_H

#include <atomic>
#include "noncopyable.h"
#include "Poll.h"
namespace yvent
{
class Poll;
class EventLoop:public noncopyable
{
public:
    EventLoop();
    ~EventLoop();

    void loop();
    bool isInLoopThread();
    void updateChannel(Channel* channel);
    void quit();
private:
    const pid_t tid_;
    std::atomic_bool quit_;
    Poll poll_;

private:
    Poll::ChannelList activeChannels_;
};

}//namespace yvent



#endif //YVENT_EVENTLOOP_H