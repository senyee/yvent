#include <iostream>
#include <thread>
#include <stdio.h>
#include <unistd.h>
#include <unistd.h>	// syscall()
#include <syscall.h> // SYS_gettid
#include "yvent/EventLoop.h"
#include "yvent/http/HttpServer.h"

#include "gtest/gtest.h"
using namespace yvent;
namespace {

// The fixture for testing class EventLoopTest.
class HttpServerTest : public ::testing::Test {
 protected:
  // You can remove any or all of the following functions if its body
  // is empty.

    HttpServerTest() {
     // You can do set-up work for each test here.
    }

    ~HttpServerTest() override {
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


TEST_F(HttpServerTest,server) {
    //setLogLevel(0);
    EventLoop loop;
    ASSERT_TRUE(loop.isInLoopThread());
    InetAddr host(1234);
    HttpServer httpServer(&loop, host);
    httpServer.start();
    loop.loop();
    //connector.start();
    //netcat -l -p 1234
    //loop.loop();
}



}  // namespace
