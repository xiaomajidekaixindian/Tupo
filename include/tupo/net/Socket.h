#include <sys/socket.h>
#include <unistd.h>
namespace Tupo {
namespace net {

class Socket {
public:
  explicit Socket(int sockfd) : sockfd_(sockfd) {}
  int fd() const { return sockfd_; }
  void close();
  void bind(const struct sockaddr *addr, socklen_t addrlen);
  void listen();
  int accept(struct sockaddr *addr, socklen_t *addrlen);
  // 地址重用
  void setReuseAddr(bool on);

  // 端口重用
  void setReusePort(bool on);

  // TCP_NODELAY，保证实时性
  void setTcpNoDelay(bool on);

  // 保持连接
  void setKeepAlive(bool on);

private:
  void setSockOpt(int level, int optname, const void *optval, socklen_t optlen);
  int sockfd_;
};
} // namespace net
} // namespace Tupo