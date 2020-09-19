#ifndef YVENT_HTTPREQUEST_H
#define YVENT_HTTPREQUEST_H
#include <string>
#include <map>
#include <cassert>
namespace yvent
{

class HttpRequest
{
public:
    enum Method
    {
        Invalid, Get, Post, Head, Put, Delete
    };
    enum Version
    {
        Unknown, Http10, Http11
    };

    HttpRequest();
    void setVersion(Version v)
        { version_ = v; }

    Version version() const
        { return version_; }

    bool setMethod(const char* start, const char* end);

    Method method() const
        { return method_; }

    const char* methodString() const;

    void setPath(const char* start, const char* end);
        //{ path_.assign(start, end); }

    const std::string path() const
        { return path_; }

    void setQuery(const char* start, const char* end)
        { query_.assign(start, end); }

    const std::string query() const
        { return query_; }
    
    void addHeader(const char* start, const char* colon, const char* end);

    std::string getHeader(const std::string& field) const;

    const std::map<std::string, std::string>& headers() const
        { return headers_; }

    void swap(HttpRequest& that)
    {
        std::swap(method_, that.method_);
        std::swap(version_, that.version_);
        path_.swap(that.path_);
        query_.swap(that.query_);
        headers_.swap(that.headers_);
    }
private:
    Method method_;
    Version version_;
    std::string path_;
    std::string query_;
    std::map<std::string, std::string> headers_;
};



} // namespace yvent


#endif //YVENT_HTTPREQUEST_H