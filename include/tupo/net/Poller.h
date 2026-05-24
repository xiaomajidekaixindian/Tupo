#pragma once
#include "tupo/net/EventLoop.h"
#include <unordered_map>
#include <vector>
struct pollfd;
namespace Tupo {
namespace net {

class Channel;

class Poller {
public:
  explicit Poller(Tupo::net::EventLoop *loop);
  virtual ~Poller();
  Poller(const Poller &) = delete;
  Poller &operator=(const Poller &) = delete;
  Poller(const Poller &&) = delete;
  Poller &&operator=(const Poller &&) = delete;

  static std::unique_ptr<Poller> newDefaultPoller(EventLoop *loop);

  typedef std::vector<Channel *> ChannelList;

  // 轮询I/O事件，必须在EventLoop线程调用
  virtual void poll(int timeout, ChannelList *activeChannels) = 0;

  // 更新Channel关心事件，必须在EventLoop线程调用
  virtual void updateChannel(Channel *channel) = 0;

  // 移除Channel，必须在EventLoop线程中调用
  virtual void removeChannel(Channel *channel) = 0;

  // 检查Channel是否在当前Poller中
  virtual bool hasChannel(Channel *channel) const;

  // 检查Poll是否在当前线程
  void assertInLoopThread() const { ownerLoop_->assertInLoopThread(); }

protected:
  // 保存fd到Channel映射
  typedef std::unordered_map<int, Channel *> ChannelMap;

  ChannelMap channels_; // 所有注册的Channel
private:
  Tupo::net::EventLoop *ownerLoop_;
};
} // namespace net
} // namespace Tupo