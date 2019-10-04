#include <iostream>
#include <thread>
#include "yvent/EventLoop.h"
#include "yvent/Channel.h"
#include "yvent/InetAddr.h"
#include "yvent/Acceptor.h"
#include "gtest/gtest.h"
using namespace yvent;
namespace {

// The fixture for testing class EventLoopTest.
class EventLoopTest : public ::testing::Test {
 protected:
  // You can remove any or all of the following functions if its body
  // is empty.

    EventLoopTest() {
     // You can do set-up work for each test here.
    }

    ~EventLoopTest() override {
     // You can do clean-up work that doesn't throw exceptions here.
    }

    // If the constructor and destructor are not enough for setting up
    // and cleaning up each test, you can define the following methods:

    void SetUp() override {
     // Code here will be called immediately after the constructor (right
     // before each test).
    }

     void TearDown() override {
     // Code here will be called immediately after each test (right
     // before the destructor).
    }

    // Objects declared here can be used by all tests in the test suite for Foo.
};

TEST_F(EventLoopTest, loop) {

   std::thread loopThread([](){
        yvent::EventLoop loop;
        yvent::Channel channel(&loop, 0);
        channel.enableRead();
        ASSERT_TRUE(loop.isInLoopThread());
        //loop.loop();
    });
    loopThread.join();
}

TEST_F(EventLoopTest, isInloopThread) {
    yvent::EventLoop loop;
    ASSERT_TRUE(loop.isInLoopThread());

    std::thread loopThread([&](){
        ASSERT_FALSE(loop.isInLoopThread());
    });
    loopThread.join();
}

TEST_F(EventLoopTest, listen) {
    EventLoop loop;
    ASSERT_TRUE(loop.isInLoopThread());

    InetAddr host(1234);
    Acceptor acceptor(&loop, host);
    acceptor.listen();
    loop.loop();
}

}  // namespace
