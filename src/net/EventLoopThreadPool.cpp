#include "tupo/net/EventLoopThreadPool.h"
#include <iostream>
namespace Tupo {
namespace net {

EventLoopThreadPool::EventLoopThreadPool(EventLoop *baseLoop, int numThreads)
    : baseLoop_(baseLoop), started_(false), numThreads_(numThreads), next_(0) {}

EventLoopThreadPool::~EventLoopThreadPool() {}

void EventLoopThreadPool::start() {
  if (started_)
    return;
  started_ = true;
  for (int i = 0; i < numThreads_; ++i) {
    auto eventLoopThread =
        std::make_unique<EventLoopThread>(threadInitCallback_);
    loops_.push_back(eventLoopThread->startLoop());
    threads_.push_back(std::move(eventLoopThread));
  }
}

EventLoop *EventLoopThreadPool::getNextLoop() {
  EventLoop *loop = baseLoop_;
  if (!loops_.empty()) {
    loop = loops_[next_];
    ++next_;
    if (next_ >= static_cast<int>(loops_.size())) {
      next_ = 0;
    }
  }
  return loop;
}
} // namespace net
} // namespace Tupo
