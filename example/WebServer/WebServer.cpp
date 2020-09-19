#include <stdio.h>
#include <set>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/sendfile.h>
#include "yvent/EventLoop.h"
#include "yvent/TcpServer.h"
#include "yvent/http/HttpServer.h"
#include "yvent/http/HttpResponse.h"
#include "yvent/http/HttpRequest.h"

using namespace yvent;

struct Location
{
    std::string rootPath;
    int dirfd;
    std::vector<std::string> indexs;
};

class WebServer
{
public:
    WebServer(EventLoop* loop, const InetAddr& server);
    ~WebServer();
    void start()
    {
        server_.start();
    }
private:
    void onRequest(const TcpConnectionPtr& conn, const HttpRequest& req, HttpResponse* rsp)
    {
        std::string path;
        std::string suffix;
        std::string::size_type pos;
        pos = req.path().rfind('.');
        if (pos != std::string::npos) {
            suffix = req.path().substr(pos);
        }
        if(req.path().length() == 1) {
            path = "./";
        } else {
            path = req.path().substr(1);
        }
        struct stat stats;
        int err = fstatat(location_.dirfd, path.c_str(), &stats, 0);
        if (err == -1) {
            LOG_SYSERR("fstatat %s error", path.c_str());
            rsp->setStatusCode(HttpResponse::k404NotFound);
            return;
        }
        if (stats.st_mode & S_IFDIR) {
            if(path[path.size()-1] != '/') {
                path += '/';
            }
            bool found = false;
            for(size_t i = 0; i < location_.indexs.size(); ++i) {
                std::string pathPre = path;
                pathPre += location_.indexs[i];
                err = fstatat(location_.dirfd, pathPre.c_str(), &stats, 0);
                if (err != -1) {
                    found = true;
                    pos = pathPre.rfind('.');
                    if (pos != std::string::npos) {
                        path = pathPre;
                        suffix = pathPre.substr(pos);
                    }
                    break;
                }
            }
            if(!found) {
                rsp->setStatusCode(HttpResponse::k404NotFound);
                return;
            }
        }
        int fd = openat(location_.dirfd, path.c_str(), O_RDONLY);
        
        conn->setSendFd(fd, stats);
        rsp->setBodyLen(stats.st_size);
        auto it = contentTypes.find(suffix.substr(1));
        if(it != contentTypes.end()) {
            rsp->setContentType(it->second);
        }
    }
    void errProcess(const TcpConnectionPtr& conn, const HttpRequest& req, HttpResponse* rsp)
    {
        rsp->setContentType("text/html");
        char body[256];
        int dataLen = 0;
        int n = sprintf(body, "<html><head><title>%d %s</title></head>\n", 404, "hello");
        dataLen += n;
        n = sprintf(body + dataLen, "<body bgcolor=\"#cc99cc\"><h4>%d %s</h4>\n", 404, "web");
        dataLen += n;
        n = sprintf(body + dataLen,"%s\n", "hahahah");
        dataLen += n;
        n = sprintf(body + dataLen,"<hr>\n</body>\n</html>\n");
        dataLen += n;
        rsp->appendBody(body);
    }

private:
    typedef std::set<TcpConnectionPtr> ConnectionList;
    HttpServer server_;
    Location location_;
    std::map<std::string, std::string> contentTypes;
};

WebServer::WebServer(EventLoop* loop, const InetAddr& server):
            server_(loop, server)
{
    server_.setHttpCallback([this](const TcpConnectionPtr& conn, const HttpRequest& req, HttpResponse* rsp){
        this->onRequest(conn, req, rsp);
    });
    location_.rootPath = "/mnt/e/bloghexo/blog/public";
    location_.dirfd = open(location_.rootPath.c_str(), O_DIRECTORY | O_RDONLY);
    location_.indexs.push_back("index.html");
    location_.indexs.push_back("index.htm");
    if (location_.dirfd == -1) {
        fprintf(stderr, "configure %s error: %s", location_.rootPath.c_str(), strerror(errno));
        exit(EXIT_FAILURE);
    }
    contentTypes = {
        {"html", "text/html; charset=utf-8"},
        {"txt", "text/plain; charset=utf-8"},
        {"xml", "text/xml"},
        {"asp", "text/asp"},
        {"css", "text/css"},
        {"gif", "image/gif"},
        {"ico", "image/x-icon"},
        {"png", "image/png"},
        {"jpg", "image/jpeg"},
        {"js" , "application/javascript"},
        {"pdf", "application/pdf"}
    };

}

WebServer::~WebServer()
{
    close(location_.dirfd);
}

int main(int argc, char* argv[])
{
    if (daemon(1, 0) == -1) {
            LOG_SYSERR("daemonize error");
            exit(EXIT_FAILURE);
    }
    EventLoop loop;
    InetAddr server(1234);
    WebServer WebServer(&loop, server);
    WebServer.start();
    loop.loop();

    return 0;
}