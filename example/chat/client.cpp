#include <stdio.h>
#include <set>
#include <iostream>
#include <string>
#include "yvent/EventLoop.h"
#include "yvent/TcpClient.h"
#include "yvent/EventLoopThread.h"

using namespace yvent;

class ChatClient
{
public:
    ChatClient(EventLoop* loop, const InetAddr& server);
    ~ChatClient();
    void start()
    {
        client_.start();
    }
    void send(const std::string& message)
    {
        client_.send(message);
    }
private:
    void onConnection(const TcpConnectionPtr& conn)
    {
        std::cout << "new connection:" << conn->name() << std::endl;
    }
    void onMessage(const TcpConnectionPtr& conn, Buffer* buffer)
    {
        std::cout << conn->name() << ":\n\t" << buffer->retrieveAllAsString() << std::endl;
    }
private:
    TcpClient client_;
};

ChatClient::ChatClient(EventLoop* loop, const InetAddr& server):
            client_(loop, server)
{
    client_.setMessageCallback([this](const TcpConnectionPtr& conn, Buffer* buffer){
        this->onMessage(conn, buffer);
    });
    client_.setConnectionCallback([this](const TcpConnectionPtr& conn){
        this->onConnection(conn);
    });
}

ChatClient::~ChatClient()
{
}

int main(int argc, char* argv[])
{
    EventLoopThread loopThread(nullptr);
    InetAddr server(1234);
    ChatClient chatClient(loopThread.starLoop(), server);
    chatClient.start();
    std::string strLine;
    while (std::getline(std::cin, strLine)) {
        chatClient.send(strLine);
    }
    return 0;
}