#include <algorithm>
#include "HttpContext.h"
#include "yvent/Buffer.h"
using namespace yvent;

bool HttpContext::parseRequestLine(const char* begin, const char* end)
{
    bool result = false;
    const char* start = begin;
    const char* space = std::find(start, end, ' ');
    if (space != end && request_.setMethod(start, space)) {
        start = space+1;
        space = std::find(start, end, ' ');
        if (space != end) {
            const char* question = std::find(start, space, '?');
            if (question != space)
            {
                request_.setPath(start, question);
                request_.setQuery(question, space);
            }
            else
            {
                request_.setPath(start, space);
            }
            start = space+1;
            result = end-start == 8 && std::equal(start, end-1, "HTTP/1.");
            if (result) {
                char c = *(end-1);
                if (c == '0') {
                    request_.setVersion(HttpRequest::Http10);
                } else if (c == '1') {
                    request_.setVersion(HttpRequest::Http11);
                } else {
                    result = false;
                }
            }
        }
    }

    return result;
}

bool HttpContext::parseRequest(Buffer* buffer)
{
    bool result = true;
    bool hasMore = true;
    while (hasMore) {
        switch (state_) {
        case RequestLine:
        {
            const char* crlf = buffer->findCRLF();
            if (crlf) {
                result = parseRequestLine(buffer->peek(), crlf);
                if (result) {
                    buffer->retrieveUntil(crlf+2);
                    state_ = Headers;
                } else {
                    hasMore = false;
                }
            } else {
                hasMore = false;
            }
            break;
        }
        case Headers:
        {
            const char* crlf = buffer->findCRLF();
            if (crlf) {
                const char* colon = std::find(buffer->peek(), crlf, ':');
                if (colon != crlf) {
                    request_.addHeader(buffer->peek(), colon, crlf);
                } else {
                    state_ = AllDone;
                    hasMore = false;
                }
                buffer->retrieveUntil(crlf+2);
            } else {
                hasMore = false;
            }
            break;
        }
        case Body:
            break;
        default:
            break;
        }
    }
    return result;
}