#include <sys/timerfd.h>
#include <unistd.h>
#include <strings.h>
#include <cassert>
#include "TimerQueue.h"
#include "Logging.h"
#include "Timer.h"
#include "EventLoop.h"
using namespace yvent;
using namespace std::literals::chrono_literals;
TimerQueue::TimerQueue(EventLoop *loop)
    :loop_(loop),
    timerfd_( ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC) ),
    timerChannel_(loop, timerfd_)
{
    LOG_TRACE("timerfd=%d", timerfd_);
    if(timerfd_ < 0)
        LOG_TRACE("timerfd_create err");
    assert(loop_->isInLoopThread());
    timerChannel_.setReadCallback([this](){this->handleRead();});
    timerChannel_.enableRead();
}

TimerQueue::~TimerQueue()
{
    for (auto& timerEntry: timers_)
        delete timerEntry.second;

    ::close(timerfd_);
}

Timer* TimerQueue::addTimer(const TimePoint &when, TimerCallback timerCallback, Interval interval)
{
    Timer *pTimer = new Timer(when, timerCallback, interval);
    loop_->runInLoop([&](){
            auto timerEntry = this->timers_.insert( make_pair(when, pTimer) );
            if(timers_.begin() == timerEntry.first) {
                this->setTimer(when);
            }
        });
    return pTimer;
}

void TimerQueue::setTimer(const TimePoint &when)
{
    Interval duration = when - TimePoint::clock::now();
    if(duration < 1ms) duration = 1ms;
    struct timespec time;
    time.tv_sec = static_cast<time_t>(duration.count() / std::nano::den);
    time.tv_nsec = duration.count() % std::nano::den;

    struct itimerspec newTime, oldTime;
    ::bzero(&newTime, sizeof(newTime));
    ::bzero(&oldTime, sizeof(oldTime));
    newTime.it_value = time;
    int ret = ::timerfd_settime(timerfd_, 0, &newTime, &oldTime);
    if(-1 == ret) {
        LOG_SYSERR("timerfd_settime");
    }
}

void TimerQueue::handleRead()
{
    uint64_t val;
    ssize_t n = read(timerfd_, &val, sizeof(val));
    if (n != sizeof(val))
        LOG_SYSERR("timerfdRead %ld, excpect %lu", n, sizeof(val));
    
    for (auto& e: getExpired()) {
        Timer* timer = e.second;
        timer->run();
        delete timer;
    }
    
}

std::vector<TimerQueue::TimerEntry> TimerQueue::getExpired()
{
    TimePoint now(std::chrono::system_clock::now());

    TimerEntry en(now + 1ns, nullptr);
    std::vector<TimerEntry> entries;

    auto end = timers_.lower_bound(en);
    entries.assign(timers_.begin(), end);
    timers_.erase(timers_.begin(), end);

    return entries;

}