#include "tupo/net/EventLoopThread.h"
#include "tupo/net/EventLoop.h"
#include <iostream>

namespace Tupo {
namespace net {
EventLoopThread::EventLoopThread()
    : exiting_(false), thread_([this]() { threadFunc(); }), loop_(nullptr) {}

EventLoopThread::EventLoopThread(ThreadFunc func)
    : exiting_(false), thread_([this]() { threadFunc(); }), loop_(nullptr),
      func_(func) {}

EventLoopThread::EventLoopThread(ThreadFunc func, const std::string &name)
    : exiting_(false), thread_([this]() { threadFunc(); }), loop_(nullptr),
      func_(func), name_(name) {}

EventLoopThread::~EventLoopThread() {
  exiting_ = true;
  if (loop_ != nullptr)
    loop_->quit();                              // 先叫停
  if (thread_.started() && !thread_.joined()) { // 启动过就必须 join
    thread_.join();
  }
}

EventLoop *EventLoopThread::startLoop() {
  thread_.start();
  {
    std::unique_lock<std::mutex> lock(mutex_.getMutex());
    while (loop_ == nullptr) {
      cond_.wait(lock);
    }
  }
  return loop_;
}

void EventLoopThread::threadFunc() {
  EventLoop loop;
  if (func_) {
    func_(&loop);
  }

  {
    Tupo::base::MutexLockGuard lock(mutex_);
    loop_ = &loop;
  }
  cond_.notify_one();

  loop.loop();
}
} // namespace net
} // namespace Tupo