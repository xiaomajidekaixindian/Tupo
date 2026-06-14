#include "tupo/net/Socket.h"
#include <cstring>
#include <iostream>
namespace Tupo {
namespace net {

void Socket::close() {
  if (sockfd_ >= 0) {
    ::close(sockfd_);
  }
}

void Socket::bind(const struct sockaddr *addr, socklen_t addrlen) {
  if (::bind(sockfd_, addr, addrlen) < 0) {
    // 处理绑定错误
    // 可以抛出异常或者记录日志
    std::cerr << "Socket bind error: " << strerror(errno) << std::endl;
  }
}

void Socket::listen() {
  if (::listen(sockfd_, SOMAXCONN) < 0) {
    // 处理监听错误
    // 可以抛出异常或者记录日志
    std::cerr << "Socket listen error: " << strerror(errno) << std::endl;
  }
}

int Socket::accept(struct sockaddr *addr, socklen_t *addrlen) {
  int connfd = ::accept(sockfd_, addr, addrlen);
  if (connfd < 0) {
    // 处理接受连接错误
    // 可以抛出异常或者记录日志
    std::cerr << "Socket accept error: " << strerror(errno) << std::endl;
  }
  return connfd;
}
} // namespace net
} // namespace Tupo