#ifndef YVENT_TIMERQUEUE_H
#define YVENT_TIMERQUEUE_H
#include <set>
#include <memory>
#include "Timer.h"
#include "Callbacks.h"
#include "Channel.h"
namespace yvent
{
class EventLoop;
class TimerQueue
{
public:
    TimerQueue(EventLoop *loop);
    ~TimerQueue();
    Timer* addTimer(const TimePoint &when, TimerCallback timerCallback, Interval interval);
    void cancelTimer(Timer* timer);
private:
    typedef std::pair<TimePoint, Timer*> TimerEntry;
    void setTimer(const TimePoint &when);
    void handleRead();
    std::vector<TimerEntry> getExpired();
private:
    
    EventLoop *loop_;
    int timerfd_;
    Channel timerChannel_;
    std::set<TimerEntry> timers_;
};



    
} // namespace yvent


#endif //YVENT_TIMERQUEUE_H