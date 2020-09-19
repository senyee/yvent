#include "HttpRequest.h"

using namespace yvent;



HttpRequest::HttpRequest():method_(Invalid), version_(Unknown)
{
}

bool HttpRequest::setMethod(const char* start, const char* end)
{
    assert(method_ == Invalid);
    std::string m(start, end);
    if (m == "GET")
    {
      method_ = Get;
    }
    else if (m == "POST")
    {
      method_ = Post;
    }
    else if (m == "HEAD")
    {
      method_ = Head;
    }
    else if (m == "PUT")
    {
      method_ = Put;
    }
    else if (m == "DELETE")
    {
      method_ = Delete;
    }
    else
    {
      method_ = Invalid;
    }
    return method_ != Invalid;
}

void HttpRequest::setPath(const char* start, const char* end)
{
    enum { 
          kStart = 0,
          kPercent
    } state = kStart;

    const char* p = start;
    int hex1, hex2;
    for(; p < end; ++p) {
        char c = *p;
        switch (state) {
            case kStart:
                if( c == '%') {
                    state = kPercent;
                } else {
                    path_ += c; 
                }
                break;
            case kPercent:
                if (isdigit(c)) {
                    hex1 = c - '0';
                }
                else {
                    hex1 = (c | 0x20);//转小写
                    if (hex1 >= 'a' && hex1 <= 'f') {
                        hex1 = hex1 - 'a' + 10; //字母转数字
                    } else {
                        break;
                    }
                }

                c = *++p;

                if (isdigit(c)) {
                    hex2 = c - '0';
                }
                else {
                    hex2 = (c | 0x20);
                    if (hex2 >= 'a' && hex2 <= 'f') {
                        hex2 = hex2 - 'a' + 10;
                    } else {
                        break;
                    }
                }

                path_ += static_cast<char>((hex1 << 4) + hex2);
                state = kStart;
                break;
            default:
                break;
        }
    }
}

const char* HttpRequest::methodString() const
{
    const char* result = "UNKNOWN";
    switch(method_)
    {
      case Get:
        result = "GET";
        break;
      case Post:
        result = "POST";
        break;
      case Head:
        result = "HEAD";
        break;
      case Put:
        result = "PUT";
        break;
      case Delete:
        result = "DELETE";
        break;
      default:
        break;
    }
    return result;
}

void HttpRequest::addHeader(const char* start, const char* colon, const char* end)
{
    std::string key(start, colon);
    ++colon;
    while (colon < end && isspace(*colon)) {
        ++colon;
    }
    std::string value(colon, end);
    while (!value.empty() && isspace(value[value.size()-1]))
    {
      value.resize(value.size()-1);
    }

    headers_[key] = value;
}

std::string HttpRequest::getHeader(const std::string& key) const
{
    std::string value;
    auto it = headers_.find(key);
    if (it != headers_.end()) {
        value = it->second;
    }
    return value;
}
