
#ifndef YVENT_HTTPRESPONSE_H
#define YVENT_HTTPRESPONSE_H

#include <map>
#include <string>

namespace yvent
{

class Buffer;
class HttpResponse
{
 public:
    enum HttpStatusCode
    {
        kUnknown,
        k200Ok = 200,
        k301MovedPermanently = 301,
        k400BadRequest = 400,
        k404NotFound = 404,
    };

    explicit HttpResponse(bool close)
    : statusCode_(kUnknown),
        closeConnection_(close),
        bodyLen_(0)
    {
    }

    void setStatusCode(HttpStatusCode code)
        { statusCode_ = code; }

    void setStatusMessage(const std::string& message)
        { statusMessage_ = message; }

    void setCloseConnection(bool on)
        { closeConnection_ = on; }

    bool closeConnection() const
        { return closeConnection_; }

    void setContentType(const std::string& contentType)
        { addHeader("Content-Type", contentType); }

    void addHeader(const std::string& key, const std::string& value)
        { headers_[key] = value; }

    void setBody(const std::string& body)
        { body_ = body; }

    void appendBody(const std::string& body)
        { body_ += body; }

    void appendToBuffer(Buffer* output) const;

    void setBodyLen(const size_t bodyLen)
        { bodyLen_ = bodyLen; }
private:
    HttpStatusCode statusCode_;
    std::map<std::string, std::string> headers_;
    std::string statusMessage_;
    bool closeConnection_;
    std::string body_;
    size_t bodyLen_;
};

}  // namespace yvent

#endif  // YVENT_HTTPRESPONSE_H
