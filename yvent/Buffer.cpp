
#include <cerrno>
#include <sys/uio.h>
#include "Buffer.h"
#include "Logging.h"

using namespace yvent;

Buffer::Buffer(size_t initialSize /*= kInitialSize*/)
            :buffer_(kCheapPrepend + kInitialSize),
             readerIndex_(kCheapPrepend),
             writerIndex_(kCheapPrepend)
{

}
    
// default copy-ctor, dtor and assignment are fine

void Buffer::swap(Buffer& buf)
{
    buffer_.swap(buf.buffer_);
    std::swap(readerIndex_, buf.readerIndex_);
    std::swap(writerIndex_, buf.writerIndex_);
}

