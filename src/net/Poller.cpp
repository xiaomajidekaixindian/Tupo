#include "tupo/net/Poller.h"
#include "tupo/net/Channel.h"
#include "tupo/net/poller/EpollPoller.h"
#include "tupo/net/poller/PollPoller.h"
#include <iostream>
namespace Tupo {
namespace net {

Poller::Poller(Tupo::net::EventLoop *loop) : ownerLoop_(loop) {}

Poller::~Poller() {}

std::unique_ptr<Poller> Poller::newDefaultPoller(EventLoop *loop) {

  // 自动检测，优先使用epoll
#ifdef __linux__
  std::cout << "Using EPollPoller (auto-detected Linux)" << std::endl;
  return std::make_unique<EpollPoller>(loop);
#else
  std::cout << "Using PollPoller (fallback for non-Linux)" << std::endl;
  return std::make_unique<PollPoller>(loop);
#endif
}

bool Poller::hasChannel(Channel *channel) const {
  assertInLoopThread();
  if (!channel)
    return false;

  auto it = channels_.find(channel->fd());
  if (it == channels_.end()) {
    return false;
  }
  return it->second == channel;
}

} // namespace net
} // namespace Tupo
