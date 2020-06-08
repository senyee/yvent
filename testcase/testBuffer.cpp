#include <iostream>
#include <thread>
#include <stdio.h>
#include <unistd.h>
#include "yvent/Buffer.h"
#include "gtest/gtest.h"
using namespace yvent;
namespace {

// The fixture for testing class EventLoopTest.
class BufferTest : public ::testing::Test {
 protected:
  // You can remove any or all of the following functions if its body
  // is empty.

    BufferTest() {
     // You can do set-up work for each test here.
    }

    ~BufferTest() override {
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

TEST_F(BufferTest,append) {
    char buf[16] = "123456789abcdef";
    Buffer buffer(8);
    buffer.append(buf, 16);
    EXPECT_EQ(buffer.readableBytes(), 16);
    EXPECT_STREQ(buf, buffer.retrieveAllAsString().c_str());
}


}  // namespace
