#include <iostream>
#include <thread>
#include <stdio.h>
#include <unistd.h>
#include <unistd.h>	// syscall()
#include <syscall.h> // SYS_gettid
#include "yvent/EventLoop.h"
#include "yvent/InetAddr.h"
#include "yvent/TcpServer.h"
#include "yvent/TcpConnection.h"
#include "yvent/Connector.h"
#include "yvent/TcpClient.h"
#include "gtest/gtest.h"
using namespace yvent;
namespace {

// The fixture for testing class EventLoopTest.
class TcpClientTest : public ::testing::Test {
 protected:
  // You can remove any or all of the following functions if its body
  // is empty.

    TcpClientTest() {
     // You can do set-up work for each test here.
    }

    ~TcpClientTest() override {
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


TEST_F(TcpClientTest,Connector) {
    EventLoop loop;
    ASSERT_TRUE(loop.isInLoopThread());
    InetAddr host(1234);
    Connector connector(&loop, host);
    connector.setNewConnectionCallback([&](const int cfd,const InetAddr& peer){
        std::cout << peer.ip() << ":" << peer.port() << std::endl;
    });
    //connector.start();
    //netcat -l -p 1234
    //loop.loop();
}

TEST_F(TcpClientTest,Client) {
    EventLoop loop;
    ASSERT_TRUE(loop.isInLoopThread());
    InetAddr server(1234);
    TcpClient client(&loop, server);
    client.setConnectionCallback([&loop](const TcpConnectionPtr& conn){
        if (conn->connected()) {
            //conn->forceClose();
        } else {
            std::cout << "client closed" << std::endl;
        }
        
        //loop.quit();
    });
    client.start();
    //netcat -l -p 1234
    //loop.loop();
}


}  // namespace
