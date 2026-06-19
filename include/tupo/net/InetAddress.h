#include <arpa/inet.h>
#include <cstdint>
#include <cstring>
#include <netinet/in.h>
#include <string>

namespace Tupo {
namespace net {
class InetAddress {
public:
  explicit InetAddress(uint16_t port = 0, bool loopbackOnly = false);
  InetAddress(const std::string &ip, uint16_t port);
  explicit InetAddress(const struct sockaddr_in &addr);
  explicit InetAddress(const struct sockaddr_in6 &addr);

private:
  struct sockaddr_storage addr_;
};

} // namespace net
} // namespace Tupo