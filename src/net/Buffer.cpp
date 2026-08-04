#include "tupo/net/Buffer.h"

namespace Tupo {
namespace net {

Buffer::Buffer(size_t initialSize):buffer_(kCheapPrepend + initialSize), 
    readIndex_(kCheapPrepend),
    writeIndex_(kCheapPrepend){

}


void Buffer::retrieve(size_t len){
    if(len < readableBytes()){
        readIndex_ += len;
    }else{
        retrieveAll();
    }
}

void Buffer::retrieveAll(){
    readIndex_ = kCheapPrepend;
    writeIndex_ = kCheapPrepend;
}

void Buffer::append(const char* data, size_t len){
    if(writableBytes()<len){
        makeSpace(len);
    }
    std::copy(data, data+len, writablePtr());
    writeIndex_ += len;
}

void Buffer::append(const std::string& str){
    append(str.data(), str.size());
}

void Buffer::makeSpace(size_t len){
    if(writableBytes()>= len){
        return;
    }
    if(writableBytes() + prependableBytes() >= len + kCheapPrepend){
        size_t readable = readableBytes();
        std::copy(begin()+readIndex_, begin()+writeIndex_, begin()+kCheapPrepend);
        readIndex_ = kCheapPrepend;
        writeIndex_ = kCheapPrepend + readable;
    }else{
        size_t newSize = buffer_.size() * 2;
         while (newSize < writeIndex_ + len) {
            newSize *= 2;
        }
        buffer_.resize(newSize);
    }
}

ssize_t Buffer::readFd(int fd, int* savedErrno){
    char extrabuf[65536];
    struct iovec vec[2];
    const size_t writable = writableBytes();
    vec[0].iov_base = writablePtr();
    vec[0].iov_len = writable;
    vec[1].iov_base = extrabuf;
    vec[1].iov_len = sizeof extrabuf;

    const int iovcnt = (writable < sizeof(extrabuf)) ? 2 : 1;
    ssize_t n = ::readv(fd, vec, iovcnt);

    if(n < 0){
        *savedErrno = errno;
        return n;
    }

    if(static_cast<size_t>(n) <= writable){
        writeIndex_ += n;
    } else {
        writeIndex_ = buffer_.size();
        append(extrabuf, n - writable);
    }
    return n;
}

}
}