#ifndef YVENT_CODEC_H
#define YVENT_CODEC_H

#include "yvent/Buffer.h"
#include "yvent/TcpConnection.h"

class HeaderCodec : yvent::noncopyable
{
public:
    HeaderCodec(/* args */);
    ~HeaderCodec();
    void onMessage(const yvent::TcpConnectionPtr& conn, yvent::Buffer* buffer)
    {

    }
private:
    const static size_t kHeaderLen = sizeof(int32_t);
};

HeaderCodec::HeaderCodec(/* args */)
{
}

HeaderCodec::~HeaderCodec()
{
}


#endif //YVENT_CODEC_H