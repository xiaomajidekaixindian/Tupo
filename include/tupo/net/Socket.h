#pragma once

#include <netinet/in.h>  // 定义 IPPROTO_TCP
#include <netinet/tcp.h> // 定义 TCP_NODELAY
#include <sys/socket.h>
#include <unistd.h>
#include "tupo/net/InetAddress.h"

namespace Tupo {
namespace net {

class Socket {
public:
  explicit Socket(int sockfd);
  // RAII：析构时关闭 fd，避免监听/连接 fd 泄漏
  ~Socket();
  // 禁止拷贝：fd 所有权必须唯一，防止 double close
  Socket(const Socket &) = delete;
  Socket &operator=(const Socket &) = delete;
  int fd() const { return sockfd_; }
  void close();
  // 转移 fd 所有权：返回底层 fd 并置为 -1，之后 Socket 析构不再关闭它。
  // 调用者必须负责关闭返回值；这是"外部手动释放"唯一安全的入口，
  // 严禁绕过本类直接 ::close(fd())。
  int release();
  void bind(const struct sockaddr *addr, socklen_t addrlen);
  void listen();
  int accept(InetAddress *peerAddr);
  int accept(InetAddress *peerAddr, socklen_t *addrlen);
  
  // 创建一个非阻塞的 TCP socket
  static int createNonblockingOrDie();

  // 地址重用
  void setReuseAddr(bool on);

  // 端口重用
  void setReusePort(bool on);

  // TCP_NODELAY，保证实时性
  void setTcpNoDelay(bool on);

  // 保持连接
  void setKeepAlive(bool on);

  // 关闭写端
  void shutdownWrite();
private:
  void setSockOpt(int level, int optname, const void *optval, socklen_t optlen);
  int sockfd_;
};
} // namespace net
} // namespace Tupo
