#ifndef YVENT_HTTPCONTEXT_H
#define YVENT_HTTPCONTEXT_H
#include "HttpRequest.h"

namespace yvent
{

class Buffer;
class HttpContext
{
public:
    enum HttpRequestParseState
    {
        RequestLine,
        Headers,
        Body,
        AllDone,
    };

    HttpContext(): state_(RequestLine)
    {
    }

    bool parseRequest(Buffer* buffer);

    bool parseDone() const
        { return state_ == AllDone; }

    void reset()
    {
        state_ = RequestLine;
        HttpRequest dummy;
        request_.swap(dummy);
    }


    const HttpRequest& request() const
        { return request_; }

    HttpRequest& request()
        { return request_; }
private:
    bool parseRequestLine(const char* begin, const char* end);

private:
    HttpRequestParseState state_;
    HttpRequest request_;
};


} // namespace yvent


#endif //YVENT_HTTPCONTEXT_H