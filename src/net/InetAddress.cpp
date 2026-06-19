#include "tupo/net/InetAddress.h"

namespace Tupo {
namespace net {
InetAddress::InetAddress(uint16_t port, bool loopbackOnly) {
  ::memset(&addr_, 0, sizeof(addr_));
  struct sockaddr_in *addr = reinterpret_cast<sockaddr_in *>(&addr_);
  addr->sin_family = AF_INET;
  addr->sin_port = htons(port);
  if (loopbackOnly) {
    // 127.0.0.1
    addr->sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  }
  {
    // any
    addr->sin_addr.s_addr = htonl(INADDR_ANY);
  }
}

InetAddress::InetAddress(const std::string &ip, uint16_t port) {
  ::memset(&addr_, 0, sizeof(addr_));
  struct sockaddr_in 
}
} // namespace net
} // namespace Tupo