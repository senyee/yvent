#include "gtest/gtest.h"
#include <iostream>
#include <thread>
#include <stdio.h>
#include <unistd.h>
#include <sys/timerfd.h>
#include <thread>
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
    ::close(timerfd);

}

TEST(TimerQueueTest, TimerOnce) {
    EventLoop loop;
    TimerQueue tq(&loop);
    int time = 0;
    tq.addTimer(std::chrono::system_clock::now() + 1s,
        [&](){
            printf("timeout\n");
            time = 1;
            loop.quit();
        },
        0s );
    loop.loop();
    EXPECT_EQ(time, 1);
}

TEST(TimerQueueTest, TimerRepeat) {
    EventLoop loop;
    TimerQueue tq(&loop);
    static int time = 0;
    tq.addTimer(std::chrono::system_clock::now() + 1s,
        [&](){
            printf("timeout\n");
            time ++;
            if(5 == time)
                loop.quit();
        },
        1s );
    loop.loop();
    EXPECT_EQ(time, 5);
}

TEST(TimerQueueTest, TimerCancel) {
    EventLoop loop;
    TimerQueue tq(&loop);
    static int time = 0;
    Timer* timer = nullptr;
    timer = tq.addTimer(std::chrono::system_clock::now() + 1s,
        [&](){
            printf("timeout\n");
            time ++;
            if(5 == time) {
                EXPECT_NE(timer, nullptr);
                tq.cancelTimer(timer);
            }
        },
        1s );
    //10s 后强制退出loop，检查time值，来检查timer是否取消
    std::thread breakThread([&](){
        sleep(10);
        loop.quit();
    });
    loop.loop();
    EXPECT_EQ(time, 5);
    breakThread.join();
}

TEST(TimerQueueTest, EventLoopTimerOnce) {
    EventLoop loop;
    int time = 0;
    loop.runAt(TimePoint::clock::now() + 1s,
        [&](){
            printf("timeout\n");
            time = 1;
            loop.quit();
        });
    loop.loop();
    EXPECT_EQ(time, 1);
}

TEST(TimerQueueTest, EventLoopTimerAfter) {
    EventLoop loop;
    int time = 0;
    loop.runAfter(1s,
        [&](){
            printf("timeout\n");
            time = 1;
            loop.quit();
        });
    loop.loop();
    EXPECT_EQ(time, 1);
}

TEST(TimerQueueTest, EventLoopTimerRepeat) {
    EventLoop loop;
    static int time = 0;
    loop.runEvery( 1s,
        [&](){
            printf("timeout\n");
            time ++;
            if(5 == time)
                loop.quit();
        });
    loop.loop();
    EXPECT_EQ(time, 5);
}

TEST(TimerQueueTest, EventLoopTimerCancel) {
    EventLoop loop;
    static int time = 0;
    TimerId timerId = loop.runEvery(1s,
        [&](){
            printf("timeout\n");
            time ++;
            if(5 == time) {
                loop.cancelTimer(timerId);
            }
        });
    //10s 后强制退出loop，检查time值，来检查timer是否取消
    std::thread breakThread([&](){
        sleep(10);
        loop.quit();
    });
    loop.loop();
    EXPECT_EQ(time, 5);
    breakThread.join();
}



