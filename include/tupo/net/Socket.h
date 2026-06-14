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

private:
  void setSockOpt(int level, int optname, const void *optval, socklen_t optlen);
  int sockfd_;
};
} // namespace net
} // namespace Tupo