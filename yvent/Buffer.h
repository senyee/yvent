#ifndef YVENT_BUFFER_H
#define YVENT_BUFFER_H
#include <algorithm>
#include <vector>
#include <string>
#include <cassert>
#include <cstring>

namespace yvent
{

class Buffer
{
public:
    static const size_t kCheapPrepend = 8;
    static const size_t kInitialSize = 1024;

    Buffer(size_t initialSize = kInitialSize);
    
    // default copy-ctor, dtor and assignment are fine

    void swap(Buffer& buf);
    size_t writableBytes() const
        { return buffer_.size() - writerIndex_; }
        
    size_t readableBytes() const
        { return writerIndex_ - readerIndex_; }

    size_t prependableBytes() const
        { return readerIndex_; }

    const char *peek() const
        { return begin() + readerIndex_; }

    int32_t peekInt32() const
        { return *reinterpret_cast<const int32_t*>(peek()); }

    int32_t retrieve32()
    {
        assert(readableBytes() >= sizeof(int32_t));
        int32_t ret = peekInt32();
        retrieve(sizeof(int32_t));
        return ret;
    }
    void append(const std::string& str)
    {
        append(str.data(), str.size());
    }

    void append(const void* data, size_t len)
    {
        append(static_cast<const char*>(data), len);
    }

    void append(const char* data, size_t len)
    {
        ensureWritableBytes(len);
        std::copy(data, data+len, beginWrite());
        hasWritten(len);
    }

    void prepend(const void* data, size_t len)
    {
        assert(len <= prependableBytes());
        readerIndex_ -= len;
        const char* d = static_cast<const char*>(data);
        std::copy(d, d+len, begin()+readerIndex_);
    }

    void ensureWritableBytes(size_t len)
    {
        if (writableBytes() < len) {
            makeSpace(len);
        }
        assert(writableBytes() >= len);
    }

    char *beginWrite()
    {
        return begin() + writerIndex_;
    }

    const char* beginWrite() const
    { 
        return begin() + writerIndex_;
    }

    void hasWritten(size_t len)
    {
        assert(len <= writableBytes());
        writerIndex_ += len;
    }

    const char* findCRLF() const
    {
        const char* crlf = std::search(peek(), beginWrite(), kCRLF, kCRLF+2);
        return crlf == beginWrite() ? NULL : crlf;
    }

    const char* findCRLF(const char* start) const
    {
        assert(peek() <= start);
        assert(start <= beginWrite());
        const char* crlf = std::search(start, beginWrite(), kCRLF, kCRLF+2);
        return crlf == beginWrite() ? NULL : crlf;
    }

    void retrieve(size_t len)
    {
        assert(len <= readableBytes());
        if (len < readableBytes())
            readerIndex_ += len;
        else
            retrieveAll();
    }

    void retrieveAll()
    {
        readerIndex_ = kCheapPrepend;
        writerIndex_ = kCheapPrepend;
    }

    std::string retrieveAsString(size_t len)
    {
        assert(len <= readableBytes());
        std::string result(peek(), len);
        retrieve(len);
        return result;
    }

    std::string retrieveAllAsString()
    {
        return retrieveAsString(readableBytes());
    }

    void retrieveUntil(const char* end)
    {
        assert(peek() <= end);
        assert(end <= beginWrite());
        retrieve(end - peek());
    }

    ssize_t readFd(int fd, int* savedErrno);
private:
    char *begin()
    {
        return buffer_.data();
    }

    const char *begin() const
    {
        return buffer_.data();
    }

    void makeSpace(size_t len)
    {
        if(writableBytes() + prependableBytes() < len + kCheapPrepend) {
            buffer_.resize(writerIndex_ + len);
        } else {
            assert(kCheapPrepend < readerIndex_);
            size_t readable = readableBytes();
            std::copy(begin() + readerIndex_,
                      begin() + writerIndex_,
                      begin() + kCheapPrepend);
            readerIndex_ = kCheapPrepend;
            writerIndex_ = readerIndex_ + readable;
            assert(readable == readableBytes());
        }
    }
    std::vector<char> buffer_;
    size_t readerIndex_;
    size_t writerIndex_;
    static const char kCRLF[];
};

} // namespace yvent


#endif //YVENT_BUFFER_H