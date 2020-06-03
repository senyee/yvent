#include "gtest/gtest.h"
#include <iostream>
#include <thread>
#include <stdio.h>
#include <unistd.h>
#include <sys/timerfd.h>
#include "yvent/EventLoop.h"
#include "yvent/Logging.h"
#include "yvent/Channel.h"
#include "yvent/TimerQueue.h"
#include <chrono>
using namespace yvent;
TEST(TimerQueueTest, timer) {
    EventLoop loop;
    ASSERT_TRUE(loop.isInLoopThread());
    struct itimerspec newTime;
    bzero(&newTime, sizeof(newTime));
    newTime.it_value.tv_sec = 15;
    //newTime.it_interval = {0,0};
    int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    LOG_TRACE("timerfd=%d",timerfd);
    Channel channel(&loop, timerfd);
    channel.setReadCallback([](){printf("time out\n");});
    channel.enableRead();
    ::timerfd_settime(timerfd, 0, &newTime, NULL);
    //loop.loop();

}

TEST(TimerQueueTest, tq) {
    EventLoop loop;
    TimerQueue tq(&loop);

    tq.addTimer(std::chrono::system_clock::now() + 5s,[](){printf("timeout\n");}, 5s );
    //loop.loop();
}

