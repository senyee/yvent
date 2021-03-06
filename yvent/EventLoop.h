#ifndef YVENT_EVENTLOOP_H
#define YVENT_EVENTLOOP_H

#include <atomic>
#include <sys/eventfd.h>
#include <mutex>
#include <functional>
#include "noncopyable.h"
#include "Poll.h"
#include "Channel.h"
#include "TimerQueue.h"
namespace yvent
{
class Poll;
class EventLoop:public noncopyable
{
public:
    typedef std::function<void()> Task;
    EventLoop();
    ~EventLoop();

    void loop();
    bool isInLoopThread();
    void updateChannel(Channel* channel);
    void quit();
    void runInLoop(const Task &task);
    void addTask(const Task &task);
    void wakeup();

    TimerId runAt(TimePoint when, TimerCallback cb);
    TimerId runAfter(Interval interval, TimerCallback cb);
    TimerId runEvery(Interval interval, TimerCallback cb);
    void cancelTimer(TimerId timerId);
private:
    void runTasks();
    void handleRead();
private:
    const pid_t tid_;
    std::atomic_bool quit_;
    Poll poll_;

    Poll::ChannelList activeChannels_;
    int wakeupfd_;
    std::vector<Task> tasks_;
    std::mutex mutex_;
    std::atomic_bool runningTasks_;
    Channel channel_;
    bool looping_;
    TimerQueue timerQueue_;
};

}//namespace yvent



#endif //YVENT_EVENTLOOP_H