#ifndef YVENT_TIMER_H
#define YVENT_TIMER_H
#include <chrono>
#include <functional>
#include "Logging.h"
#include "noncopyable.h"

namespace yvent
{
    
using std::chrono::system_clock;
using namespace std::literals::chrono_literals;
typedef std::chrono::nanoseconds   Interval;
typedef std::chrono::nanoseconds   Nanosecond;
typedef std::chrono::microseconds  Microsecond;
typedef std::chrono::milliseconds  Millisecond;
typedef std::chrono::seconds       Second;
typedef std::chrono::minutes       Minute;
typedef std::chrono::hours         Hour;
typedef std::chrono::time_point<std::chrono::system_clock, Interval> TimePoint;

class Timer:public noncopyable
{
typedef std::function<void()> TimerCallback;
public:
    Timer(TimePoint when, const TimerCallback &timerCallback, Interval interval);
    ~Timer();
    void run() {if(timerCallback_) timerCallback_();}
    void restart();
    bool repeat() const { return repeat_; }
    void cancel();
    bool canceled() const { return canceled_;}
    TimePoint when() const { return when_;}
private:
    TimePoint when_;
    TimerCallback timerCallback_;
    Interval interval_;
    bool repeat_;
    bool canceled_;
};

} // namespace yvent


#endif //YVNET_TIMER_H