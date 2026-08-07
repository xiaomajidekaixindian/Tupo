#include "tupo/net/Socket.h"
#include <cstring>
#include <iostream>

namespace Tupo {
namespace net {

Socket::Socket(int sockfd) : sockfd_(sockfd) {}

void Socket::close() {
  if (sockfd_ >= 0) {
    ::close(sockfd_);
  }
}

void Socket::bind(const struct sockaddr *addr, socklen_t addrlen) {
  std::cout<<"sockfd："<<sockfd_<<"，addrlen"<<addrlen<<std::endl;
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

int Socket::accept(InetAddress *peerAddr) {
  struct sockaddr_storage addr;
  socklen_t addrlen = sizeof(addr);
  int connfd = ::accept(sockfd_, reinterpret_cast<struct sockaddr *>(&addr), &addrlen);
  if (connfd < 0) {
    // 处理接受连接错误
    // 可以抛出异常或者记录日志
    std::cerr << "Socket accept error: " << strerror(errno) << std::endl;
  }
  return connfd;
}

int Socket::accept(InetAddress *peerAddr, socklen_t *addrlen) {
  struct sockaddr* sockaddr = peerAddr->getSockAddr();
  int connfd = ::accept(sockfd_,sockaddr, addrlen);
  if (connfd < 0) {
    // 处理接受连接错误
    // 可以抛出异常或者记录日志
    std::cerr << "Socket accept error: " << strerror(errno) << std::endl;
  }
  return connfd;
}

int Socket::createNonblockingOrDie() {
  // IPV4，TCP通信，非阻塞模式，执行时关闭
  int sockfd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
  if (sockfd < 0) {
    std::cerr << "Socket::createNonblockingOrDie failed" << std::endl;
    abort();
  }
  return sockfd;
}

void Socket::setSockOpt(int level, int optname, const void *optval,
                        socklen_t optlen) {
  if (::setsockopt(sockfd_, level, optname, optval, optlen) < 0) {
    // 处理设置套接字选项错误
    // 可以抛出异常或者记录日志
    std::cerr << "Socket setsockopt error: " << strerror(errno) << std::endl;
  }
}

void Socket::setReuseAddr(bool on) {
  int optval = on ? 1 : 0;
  setSockOpt(SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
}

void Socket::setReusePort(bool on) {
  int optval = on ? 1 : 0;
  setSockOpt(SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
}

void Socket::setTcpNoDelay(bool on) {
  int optval = on ? 1 : 0;
  setSockOpt(IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(optval));
}

void Socket::setKeepAlive(bool on) {
  int optval = on ? 1 : 0;
  setSockOpt(SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval));
}

void Socket::shutdownWrite(){
    if (sockfd_ > 0) {
      if (::shutdown(sockfd_, SHUT_WR) < 0) {
          // 如果已经关闭了，忽略错误
          if (errno != ENOTCONN) {
                std::cerr << "Socket::shutdownWrite error: " << strerror(errno) << std::endl;
            }
        }
    }
}
} // namespace net
} // namespace Tupo