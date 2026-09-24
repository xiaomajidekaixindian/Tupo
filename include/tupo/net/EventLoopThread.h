#pragma once
#include "tupo/base/MutexLock.h"
#include "tupo/base/Thread.h"
#include <condition_variable>
#include <functional>

namespace Tupo {
namespace net {
class EventLoop;
class EventLoopThread {
public:
  using ThreadFunc = std::function<void(EventLoop *)>;
  explicit EventLoopThread();
  explicit EventLoopThread(ThreadFunc threadInitFunc);
  explicit EventLoopThread(ThreadFunc threadInitFunc, const std::string &name);
  explicit EventLoopThread(const EventLoopThread &) = delete;
  EventLoopThread &operator=(const EventLoopThread &) = delete;
  EventLoopThread(EventLoopThread &&other) = delete;
  EventLoopThread &operator=(EventLoopThread &&other) = delete;

  ~EventLoopThread();
  EventLoop *startLoop();

private:
  void threadFunc();
  Tupo::base::MutexLock mutex_;
  std::condition_variable cond_;
  bool exiting_;
  Tupo::base::Thread thread_;
  EventLoop *loop_;
  ThreadFunc threadInitFunc_;
  std::string name_;
};
} // namespace net
} // namespace Tupo