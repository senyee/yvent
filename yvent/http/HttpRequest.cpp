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
