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
        { return buffer_.data() + readerIndex_; }

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

    void ensureWritableBytes(size_t len)
    {
        if (writableBytes() < len) {
            makeSpace(len);
        }
        assert(writableBytes() >= len);
    }

    char *beginWrite()
    {
        return buffer_.data() + writerIndex_;
    }

    void hasWritten(size_t len)
    {
        assert(len <= writableBytes());
        writerIndex_ += len;
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
private:
    void makeSpace(size_t len)
    {
        if(writableBytes() + prependableBytes() < len + kCheapPrepend) {
            buffer_.resize(writerIndex_ + len);
        } else {
            assert(kCheapPrepend < readerIndex_);
            size_t readable = readableBytes();
            std::copy(buffer_.data() + readerIndex_,
                      buffer_.data() + writerIndex_,
                      buffer_.data() + kCheapPrepend);
            readerIndex_ = kCheapPrepend;
            writerIndex_ = readerIndex_ + readable;
            assert(readable == readableBytes());
        }
    }
    std::vector<char> buffer_;
    size_t readerIndex_;
    size_t writerIndex_;
};

} // namespace yvent


#endif //YVENT_BUFFER_H