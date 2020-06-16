#ifndef YVENT_CODEC_H
#define YVENT_CODEC_H

#include <arpa/inet.h>//ntohl

#include "yvent/Buffer.h"
#include "yvent/TcpConnection.h"
#include "yvent/Logging.h"

using namespace yvent;
class LenHeaderCodec : yvent::noncopyable
{
public:
    typedef std::function<void (const TcpConnectionPtr&, const std::string& message)> StringMessageCallback;
    LenHeaderCodec(StringMessageCallback stringMessageCallback);
    ~LenHeaderCodec();
    void onMessage(const yvent::TcpConnectionPtr& conn, yvent::Buffer* buffer);
    void send(const yvent::TcpConnectionPtr& conn, const std::string message);

private:
    const static size_t kHeaderLen = sizeof(int32_t);
    StringMessageCallback stringMessageCallback_;
};

LenHeaderCodec::LenHeaderCodec(StringMessageCallback stringMessageCallback):
                stringMessageCallback_(stringMessageCallback)
{
}

LenHeaderCodec::~LenHeaderCodec()
{
}

void LenHeaderCodec::onMessage(const yvent::TcpConnectionPtr& conn, yvent::Buffer* buffer)
{
    while (buffer->readableBytes() >= kHeaderLen) {
        int32_t len = ntohl(buffer->peekInt32());
        if (len > 65536 || len < 0)
        {
            LOG_ERROR("len=%d", len);
            conn->shutdown();
            break;
        } else if (buffer->readableBytes() >= len + kHeaderLen) {
            buffer->retrieve(kHeaderLen);
            std::string message(buffer->retrieveAsString(len));
            stringMessageCallback_(conn, message);
        } else {
            break;
        }
    }
}

void LenHeaderCodec::send(const yvent::TcpConnectionPtr& conn, const std::string message)
{
    Buffer buffer;
    buffer.append(message.data(), message.size());
    int32_t len = static_cast<int32_t>(message.size());
    int32_t netLen = htonl(len);
    buffer.prepend(&netLen, sizeof netLen);
    conn->send(buffer);
}


#endif //YVENT_CODEC_H