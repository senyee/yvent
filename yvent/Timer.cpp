#include <cassert>
#include "Timer.h"

using namespace yvent;

Timer::Timer(TimePoint when, const TimerCallback &timerCallback, Interval interval)
    :when_(when),
    timerCallback_(timerCallback),
    interval_(interval),
    repeat_(interval > Interval::zero()),
    canceled_(false)
{
    LOG_INFO("add timer %p", this);
}

Timer::~Timer()
{
    LOG_INFO("delete timer %p", this);
    canceled_ = true;
}

void Timer::restart()
{
    assert(repeat_);
    when_ += interval_;
}

void Timer::cancel()
{
    assert(!canceled_);
    canceled_ = true;
}