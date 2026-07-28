#pragma once
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

  // 获取原生sockaddr，留系统调用接口
  const struct sockaddr *getSockAddr() const { return reinterpret_cast<const struct sockaddr *>(&addr_); }
  struct sockaddr *getSockAddr() { return reinterpret_cast<struct sockaddr *>(&addr_); }

  // 获取sockaddr_in
  const struct sockaddr_in *getSockAddrIn() const { return reinterpret_cast<const struct sockaddr_in *>(&addr_); }
private:
  struct sockaddr_storage addr_;
};

} // namespace net
} // namespace Tupo