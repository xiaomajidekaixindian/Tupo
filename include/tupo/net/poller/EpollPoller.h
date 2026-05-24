#pragma once
#include "tupo/net/Poller.h"
#include <sys/epoll.h>

namespace Tupo {
namespace net {
class EpollPoller : public Poller {
public:
  EpollPoller(EventLoop *loop);
  ~EpollPoller() override;

  void poll(int timeout, ChannelList *activeChannels) override;

  void updateChannel(Channel *channel) override;

  void removeChannel(Channel *channel) override;

private:
  void update(int operation, Channel *channel);
  static const int kInitEventListSize = 16;
  
  // 添加活跃的文件描述符到数组
  void findActiveChannels(int numEvents, ChannelList *activeChannels) const;

  int epollfd_;
  // 缓冲区，保存活跃的事件
  typedef std::vector<struct epoll_event> EpollFdList;
  EpollFdList events_;
};
} // namespace net
} // namespace Tupo