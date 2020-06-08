#include <iostream>
#include <thread>
#include <stdio.h>
#include <unistd.h>
#include "yvent/EventLoop.h"
#include "yvent/Channel.h"
#include "yvent/InetAddr.h"
#include "yvent/Acceptor.h"
#include "yvent/TcpServer.h"
#include "yvent/TcpConnection.h"
#include "gtest/gtest.h"
using namespace yvent;
namespace {

// The fixture for testing class EventLoopTest.
class TcpConnectionTest : public ::testing::Test {
 protected:
  // You can remove any or all of the following functions if its body
  // is empty.

    TcpConnectionTest() {
     // You can do set-up work for each test here.
    }

    ~TcpConnectionTest() override {
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

TEST_F(TcpConnectionTest,Message) {
    EventLoop loop;
    ASSERT_TRUE(loop.isInLoopThread());

    InetAddr host(1234);
    TcpServer server(&loop, host);
    server.start();
    server.setMessageCallback([](const TcpConnectionPtr&, Buffer* buffer ) {
        std::cout << buffer->retrieveAllAsString() << std::endl;
    });
    //loop.loop();
}

}  // namespace
