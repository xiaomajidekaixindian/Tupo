#include <arpa/inet.h>
#include <cstdint>
#include <cstring>
#include <netinet/in.h>
#include <string>

namespace Tupo {
namespace net {
class InetAddress {
public:
  // 服务器监听
  explicit InetAddress(uint16_t port = 0, bool loopbackOnly = false);
  
  // 客户端指定服务器连接
  InetAddress(const std::string &ip, uint16_t port);

  // 封装IPv4
  explicit InetAddress(const struct sockaddr_in &addr);

  // 封装IPv6
  explicit InetAddress(const struct sockaddr_in6 &addr);

private:
  struct sockaddr_storage addr_;
};

} // namespace net
} // namespace Tupo