#ifndef YVENT_POLL_H
#define YVENT_POLL_H

#include <vector>
#include "yvent/noncopyable.h"

namespace yvent
{

class Channel;
class EventLoop;

class Poll:public noncopyable
{
public:
    typedef std::vector<Channel*> ChannelList;

    explicit
    Poll(EventLoop* loop);
    ~Poll();

    void poll(ChannelList& channelList);
    void updateChannel(Channel* channel);
private:
    void update(int op, Channel* channel);
private:
    EventLoop* loop_;
    std::vector<struct epoll_event> events_;
    int epfd_;
};

} // namespace yvent




#endif //YVENT_POLL_H