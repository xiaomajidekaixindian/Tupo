#pragma once
#include "tupo/net/Poller.h"
#include <poll.h>
namespace Tupo {
namespace net {
class PollPoller : public Poller {
public:
  PollPoller(EventLoop *loop);
  ~PollPoller() override = default;
  PollPoller(const PollPoller &) = delete;
  PollPoller &operator=(const PollPoller &) = delete;
  PollPoller(const PollPoller &&) = delete;
  PollPoller &&operator=(const PollPoller &&) = delete;

  void poll(int timeout, ChannelList *activeChannels) override;

  void updateChannel(Channel *channel) override;

  void removeChannel(Channel *channel) override;

private:
  // 添加活跃的文件描述符到数组
  void findActiveChannels(int numEvents, ChannelList *activeChannels) const;

  // 缓冲区，保存活跃的事件
  typedef std::vector<struct pollfd> PollFdList;
  PollFdList pollfds_;
};
} // namespace net
} // namespace Tupo