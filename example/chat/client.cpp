#include <stdio.h>
#include <set>
#include <iostream>
#include <string>
#include "yvent/EventLoop.h"
#include "yvent/TcpClient.h"
#include "yvent/EventLoopThread.h"
#include "codec.h"

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
        codec_.send(connection_, message);
    }
private:
    void onConnection(const TcpConnectionPtr& conn)
    {
        if (conn->connected()) {
            connection_ = conn;
        } else {
            connection_.reset();
        }
        std::cout << "new connection:" << conn->name() << conn->connected() << std::endl;
    }
    void onMessage(const TcpConnectionPtr& conn, std::string message)
    {
        std::cout << conn->name() << ":\n\t" << message << std::endl;
    }
private:
    TcpClient client_;
    TcpConnectionPtr connection_;
    LenHeaderCodec codec_;
};

ChatClient::ChatClient(EventLoop* loop, const InetAddr& server):
            client_(loop, server),
            codec_([this](const TcpConnectionPtr& conn, std::string message){
                this->onMessage(conn, message);
            })
{
    client_.setMessageCallback([this](const TcpConnectionPtr& conn, Buffer* buffer){
        this->codec_.onMessage(conn, buffer);
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
    if (argc <= 2) {
        printf("usage:\n\t%s ip port\n", argv[0]);
        return 0;
    }
    EventLoopThread loopThread(nullptr);
    uint16_t port = static_cast<uint16_t>(atoi(argv[2]));
    InetAddr server(argv[1],port);
    ChatClient chatClient(loopThread.starLoop(), server);
    chatClient.start();
    std::string strLine;
    while (std::getline(std::cin, strLine)) {
        chatClient.send(strLine);
    }
    return 0;
}