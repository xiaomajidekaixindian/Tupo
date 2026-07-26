#include "tupo/net/Acceptor.h"

namespace Tupo {
namespace net {
Acceptor::Acceptor(EventLoop *loop) : loop_(loop) {
  // 在这里可以初始化监听套接字，绑定地址等
}
} // namespace net
} // namespace Tupo