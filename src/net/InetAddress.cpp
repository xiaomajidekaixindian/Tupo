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
  struct sockaddr_in *addr4 = reinterpret_cast<sockaddr_in *>(&addr_);
  if (inet_pton(AF_INET, ip.c_str(), &addr4->sin_addr) == 1) {
    addr4->sin_family = AF_INET;
    addr4->sin_port = htons(port);
    return;
  }
  struct sockaddr_in6 *addr6 = reinterpret_cast<struct sockaddr_in6 *>(&addr_);
  if (::inet_pton(AF_INET6, ip.c_str(), &addr6->sin6_addr) == 1) {
    addr6->sin6_family = AF_INET6;
    addr6->sin6_port = htons(port);
    return;
  }
}

InetAddress::InetAddress(const struct sockaddr_in &addr) {
  ::memset(&addr_, 0, sizeof(addr));
  struct sockaddr_in *dst = reinterpret_cast<sockaddr_in *>(&addr_);
  *dst = addr;
}

InetAddress::InetAddress(const struct sockaddr_in6 &addr) {
  ::memset(&addr_, 0, sizeof(addr));
  struct sockaddr_in6 *dst = reinterpret_cast<sockaddr_in6 *>(&addr_);
  *dst = addr;
}

} // namespace net
} // namespace Tupo