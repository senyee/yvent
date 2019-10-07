#include <iostream>
#include <thread>
#include <stdio.h>
#include <unistd.h>
#include "yvent/EventLoop.h"
#include "yvent/Channel.h"
#include "yvent/InetAddr.h"
#include "yvent/Acceptor.h"
#include "yvent/TcpServer.h"
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
        channel.setReadCallback([](){
            char buf[BUFSIZ] = {0};
            read(0,buf,BUFSIZ);
            printf("%s copy\n",buf);
        });
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
    //loop.loop();
}

// TEST(Acceptor,newConnectionCallback) {
//     EventLoop loop;
//     ASSERT_TRUE(loop.isInLoopThread());

//     InetAddr host(1234);
//     Acceptor acceptor(&loop, host);
//     acceptor.listen();
//     acceptor.setNewConnectionCallback([](int cfd, const InetAddr &peer){
//         ::write(cfd, "hello\n", 7);
//         ::close(cfd);
//     });
//     //loop.loop();

// }

TEST(TcpServer,newConnectionCallback) {
    EventLoop loop;
    ASSERT_TRUE(loop.isInLoopThread());

    InetAddr host(1234);
    TcpServer server(&loop, host);
    server.start();
    server.setMessageCallback([](char *buf,int len){
        printf("receive:%s\n",buf);
    });
    loop.loop();

}

}  // namespace
