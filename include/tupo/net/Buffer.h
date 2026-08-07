#pragma once
#include <cstddef>
#include <vector>
#include <string>
#include <sys/uio.h>

namespace Tupo {
namespace net {

class Buffer{

public:
  inline static constexpr size_t kCheapPrepend = 8;
  inline static constexpr size_t kInitialSize = 1024;

  Buffer(size_t initialSize = kInitialSize);
  ~Buffer()=default;
  Buffer(const Buffer&)=delete;
  Buffer& operator=(const Buffer&)=delete;
  Buffer(Buffer&&)=default;
  Buffer& operator=(Buffer&&)=default;

  // 可读字节（已经写入字节数）
  size_t readableBytes() const {
    return writeIndex_ - readIndex_;
  }

  // 预留空间
  size_t prependableBytes() const {
    return readIndex_;
 }

  // 读取数据（写入移动位置）
  void retrieve(size_t len);
  void retrieveAll();
  std::string retrieveAsString(size_t len);
  std::string retrieveAllAsString();


  // 写入数据
  void append(const char* data, size_t len);
  void append(const std::string& str);
  void append(const void* data, size_t len) {
    append(static_cast<const char*>(data), len);
}
  // 数据指针
  const char* readablePtr()const{
    return begin() + readIndex_;
  }

  char *writablePtr(){
    return begin() + writeIndex_;
  }

  const char *writablePtr()const{
    return begin() + writeIndex_;
  }

  ssize_t readFd(int fd, int* savedErrno);

  const char* begin() const{
    return buffer_.data();
  }

  char *begin() {
    return buffer_.data();
  }
  
private:
  // buffer空间大小
  size_t writableBytes() const {
    return buffer_.size() - writeIndex_;
  }

  // 扩容
  void makeSpace(size_t len);

  std::vector<char> buffer_;  // 缓冲区
  size_t readIndex_;          // 读索引
  size_t writeIndex_;         // 写索引
};

}
}