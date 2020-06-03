#include "Timer.h"

using namespace yvent;

Timer::Timer(TimePoint when, const TimerCallback &timerCallback, Interval interval)
    :when_(when),
    timerCallback_(timerCallback),
    interval_(interval)
{
}

Timer::~Timer()
{
}