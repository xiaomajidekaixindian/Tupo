#include "tupo/net/poller/EpollPoller.h"
#include "tupo/base/Logger.h"
#include <string.h>

namespace Tupo {
namespace net {
EpollPoller::EpollPoller(EventLoop *loop)
    : Poller(loop), epollfd_(epoll_create1(EPOLL_CLOEXEC)),
      events_(kInitEventListSize) {
  if (epollfd_ < 0) {
    LOG_ERROR << "EPollPoller::EPollPoller - epoll_create1 error: " << errno;
    abort();
  }
  LOG_DEBUG << "EPollPoller created, epollfd=" << epollfd_;
}
EpollPoller::~EpollPoller() {

  ::close(epollfd_);
  LOG_INFO << "EPollPoller destroyed";
}

void EpollPoller::poll(int timeout, ChannelList *activeChannels) {
  LOG_DEBUG << "EPollPoller::poll() waiting for events, timeout=" << timeout
            << "ms";
  // >0:有numEvents事件就绪，==0:超时了，没有事件，==-1出错
  int numEvents = ::epoll_wait(epollfd_, events_.data(),
                               static_cast<int>(events_.size()), timeout);
  int savedErrno = errno;

  if (numEvents > 0) {
    LOG_DEBUG << "EPollPoller::poll() " << numEvents << " events happened";
    findActiveChannels(numEvents, activeChannels);

    // 如果事件列表满了，扩容
    if (static_cast<int>(numEvents) == events_.size()) {
      events_.resize(events_.size() * 2);
    }
  } else if (numEvents == 0) {
    LOG_WARN << "EPollPoller::poll() nothing happened";
  } else {
    // EINTR 是信号中断，不是错误
    if (savedErrno != EINTR) {
      errno = savedErrno;
      LOG_ERROR << "EPollPoller::poll() error: " << errno;
    }
  }
}
void EpollPoller::updateChannel(Channel *channel) {
  assertInLoopThread();
  int fd = channel->fd();
  LOG_DEBUG << "EPollPoller::updateChannel() fd=" << fd
            << " events=" << channel->events();
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
  LOG_DEBUG << "EPollPoller::removeChannel() fd=" << channel->fd();

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
    LOG_ERROR << "EPollPoller::update() error: " << errno;
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
