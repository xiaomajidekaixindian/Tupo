
#include "tupo/net/poller/PollPoller.h"
#include "tupo/net/Channel.h"
#include <iostream>

namespace Tupo {
namespace net {

PollPoller::PollPoller(EventLoop *loop) : Poller(loop) {
  std::cout << "PollPoller created" << std::endl;
}

void PollPoller::poll(int timeout, ChannelList *activeChannels) {
  // 调用poll获取事件
  int numEvents = ::poll(pollfds_.data(), pollfds_.size(), timeout);
  if (numEvents > 0) {
    std::cout << "PollPoller::poll() " << numEvents << " events happened"
              << std::endl;
    findActiveChannels(numEvents, activeChannels);
  } else if (numEvents == 0) {
    std::cout << "PollPoller::poll() nothing happened" << std::endl;
  } else {
    std::cerr << "PollPoller::poll() error: " << errno << std::endl;
  }
}

void PollPoller::updateChannel(Channel *channel) {
  assertInLoopThread();
  // 判断Channel的状态
  if (channel->index() < 0) {
    assert(channels_.find(channel->fd()) == channels_.end());
    // 新的Channel，添加到pollfds_
    struct pollfd pfd;
    pfd.fd = channel->fd();
    pfd.events = static_cast<short>(channel->events());
    pfd.revents = 0;
    pollfds_.push_back(pfd);

    int index = static_cast<int>(pollfds_.size()) - 1;
    channel->set_index(index);
    channels_[pfd.fd] = channel;
    std::cout << "Added new channel - channels_.size() = " << channels_.size()
              << ", hasChannel = " << hasChannel(channel) << std::endl;
  } else { // 更新已有的Channel
    assert(channels_.find(channel->fd()) != channels_.end());
    assert(channels_[channel->fd()] == channel);
    int idx = channel->index();

    assert(idx >= 0 && idx < static_cast<int>(pollfds_.size()));
    struct pollfd &pfd = pollfds_[idx];
    assert(pfd.fd == channel->fd() || pfd.fd == -1);
    pfd.events = static_cast<short>(channel->events());
    pfd.revents = 0;
  }
}

void PollPoller::findActiveChannels(int numEvents,
                                    ChannelList *activeChannels) const {
  for (const auto &pfd : pollfds_) {
    if (numEvents <= 0) {
      break;
    }
    if (pfd.revents > 0) {
      --numEvents;
      auto it = channels_.find(pfd.fd);
      assert(it != channels_.end());

      Channel *channel = it->second;
      assert(channel->fd() == pfd.fd);

      channel->set_revents(pfd.revents);
      activeChannels->push_back(channel);
    }
  }
}

void PollPoller::removeChannel(Channel *channel) {
  assertInLoopThread();
  int fd = channel->fd();
  int idx = channel->index();
  assert(channels_.find(fd) != channels_.end());
  assert(channels_[fd] == channel);
  assert(idx >= 0 && idx < static_cast<int>(pollfds_.size()));

  // 从channels_中移除
  channels_.erase(fd);

  // 从pollfds_中移除
  if (idx == pollfds_.size() - 1) {
    pollfds_.pop_back();
  } else {
    int lastFd = pollfds_.back().fd;
    std::swap(pollfds_[idx], pollfds_.back());

    if (lastFd > 0) {
      channels_[lastFd]->set_index(idx);
    }
    pollfds_.pop_back();
  }
  channel->set_index(-1);
  channel->setAddedToLoop(false);
}

} // namespace net
} // namespace Tupo