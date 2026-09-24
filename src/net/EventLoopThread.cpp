#include "tupo/net/EventLoopThread.h"
#include "tupo/net/EventLoop.h"
#include <iostream>

namespace Tupo {
namespace net {
EventLoopThread::EventLoopThread()
    : exiting_(false), thread_([this]() { threadFunc(); }), loop_(nullptr) {}

EventLoopThread::EventLoopThread(ThreadFunc threadInitFunc)
    : exiting_(false), thread_([this]() { threadFunc(); }), loop_(nullptr),
      threadInitFunc_(threadInitFunc) {}

EventLoopThread::EventLoopThread(ThreadFunc threadInitFunc,
                                 const std::string &name)
    : exiting_(false), thread_([this]() { threadFunc(); }), loop_(nullptr),
      threadInitFunc_(threadInitFunc), name_(name) {}

EventLoopThread::~EventLoopThread() {
  exiting_ = true;
  {
    Tupo::base::MutexLockGuard lock(mutex_);
    if (loop_ != nullptr) {
      loop_->quit();
    }
  }
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
  if (threadInitFunc_) {
    threadInitFunc_(&loop);
  }

  {
    Tupo::base::MutexLockGuard lock(mutex_);
    loop_ = &loop;
  }
  cond_.notify_one();

  loop.loop();
  {
    Tupo::base::MutexLockGuard lock(mutex_);
    loop_ = nullptr;
  }
}
} // namespace net
} // namespace Tupo