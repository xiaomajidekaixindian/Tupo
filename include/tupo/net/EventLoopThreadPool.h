#pragma once
#include "tupo/net/EventLoop.h"
#include "tupo/net/EventLoopThread.h"
#include <functional>
#include <vector>
namespace Tupo {
namespace net {
class EventLoopThreadPool {
public:
  using ThreadInitCallback = std::function<void(EventLoop *)>;
  explicit EventLoopThreadPool(EventLoop *baseLoop, int numThreads = 0);
  ~EventLoopThreadPool();

  void start();
  void setThreadInitCallback(const ThreadInitCallback &cb) {
    threadInitCallback_ = cb;
  }

  EventLoop *getNextLoop();

private:
  EventLoop *baseLoop_;
  bool started_;
  int numThreads_;
  int next_;
  ThreadInitCallback threadInitCallback_;
  std::vector<std::unique_ptr<EventLoopThread>> threads_;
  std::vector<EventLoop *> loops_;
};
} // namespace net
} // namespace Tupo
