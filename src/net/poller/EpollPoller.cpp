#include "tupo/net/poller/EpollPoller.h"
#include <iostream>
#include <string.h>

namespace Tupo {
namespace net {
EpollPoller::EpollPoller(EventLoop *loop)
    : Poller(loop), epollfd_(epoll_create1(EPOLL_CLOEXEC)),
      events_(kInitEventListSize) {
  if (epollfd_ < 0) {
    std::cerr << "EPollPoller::EPollPoller - epoll_create1 error: " << errno
              << std::endl;
    abort();
  }
  std::cout << "EPollPoller created, epollfd=" << epollfd_ << std::endl;
}
EpollPoller::~EpollPoller() {

  ::close(epollfd_);
  std::cout << "EPollPoller destroyed" << std::endl;
}

void EpollPoller::poll(int timeout, ChannelList *activeChannels) {
  std::cout << "EPollPoller::poll() waiting for events, timeout=" << timeout
            << "ms" << std::endl;
  // >0:有numEvents事件就绪，==0:超时了，没有事件，==-1出错
  int numEvents = ::epoll_wait(epollfd_, events_.data(),
                               static_cast<int>(events_.size()), timeout);
  int savedErrno = errno;

  if (numEvents > 0) {
    std::cout << "EPollPoller::poll() " << numEvents << " events happened"
              << std::endl;
    findActiveChannels(numEvents, activeChannels);

    // 如果事件列表满了，扩容
    if (static_cast<int>(numEvents) == events_.size()) {
      events_.resize(events_.size() * 2);
    }
  } else if (numEvents == 0) {
    std::cout << "EPollPoller::poll() nothing happened" << std::endl;
  } else {
    // EINTR 是信号中断，不是错误
    if (savedErrno != EINTR) {
      errno = savedErrno;
      std::cerr << "EPollPoller::poll() error: " << errno << std::endl;
    }
  }
}
void EpollPoller::updateChannel(Channel *channel) {
  assertInLoopThread();
  int fd = channel->fd();
  std::cout << "EPollPoller::updateChannel() fd=" << fd
            << " events=" << channel->events() << std::endl;
  // 检查是否已经管理这个Channel
  bool isManaged = (channels_.find(fd) != channels_.end());
  if (!isManaged) {
    // 新的Channel，添加到epoll
    assert(channel->index() == -1); // 应该是初始状态
    channels_[fd] = channel;
    update(EPOLL_CTL_ADD, channel);
  } else {
    // 更新已有的Channel
    assert(channels_.find(channel->fd()) != channels_.end());
    assert(channels_[channel->fd()] == channel);
    if (channel->isNoneEvent()) {
      update(EPOLL_CTL_DEL,
             channel); // 不关心任何事件，从epoll中删除，保留在channel_中
    } else {
      update(EPOLL_CTL_MOD, channel);
    }
  }
}

void EpollPoller::removeChannel(Channel *channel) {
  assertInLoopThread();
  std::cout << "EPollPoller::removeChannel() fd=" << channel->fd() << std::endl;

  int fd = channel->fd();

  assert(channels_.find(fd) != channels_.end());
  assert(channels_[fd] == channel);

  // 如果还在 epoll 中，先删除
  if (!channel->isNoneEvent()) {
    update(EPOLL_CTL_DEL, channel);
  }
  // 从管理列表中移除
  size_t n = channels_.erase(fd);
  assert(n == 1);
  channel->setAddedToLoop(false);
}

void EpollPoller::update(int operation, Channel *channel) {
  struct epoll_event event;
  memset(&event, 0, sizeof(event));
  event.events = channel->events();
  event.data.ptr = channel; // 关键：保存Channel指针
  int fd = channel->fd();

  if (epoll_ctl(epollfd_, operation, fd, &event) < 0) {
    std::cerr << "operation:" << operation << ",fd=" << fd << ",error:" << errno
              << std::endl;
  }
}

void EpollPoller::findActiveChannels(int numEvents,
                                     ChannelList *activeChannel) const {
  for (int i = 0; i < numEvents; i++) {
    Channel *channel = static_cast<Channel *>(events_[i].data.ptr);
#ifndef NDEBUG
    int fd = channel->fd();
    auto it = channels_.find(fd);
    assert(it != channels_.end());
    assert(it->second == channel);
#endif
    channel->set_revents(events_[i].events);
    activeChannel->push_back(channel);
  }
}

} // namespace net
} // namespace Tupo
