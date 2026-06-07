#include "tupo/base/Thread.h"
#include <future>
#include <iostream>

namespace Tupo {
namespace base {

Thread::Thread(ThreadFunc func, const std::string &name)
    : started_(false), joined_(false), tid_(0), func_(std::move(func)),
      detached_(false) {}
Thread::~Thread() {
  if (started_ && !joined_ && thread_.joinable()) {
    thread_.detach();
  }
}
void Thread::start() {
  assert(!started_);
  started_ = true;

  std::promise<pid_t> promise;
  std::future<pid_t> future = promise.get_future();

  // 使用过lambda捕获this
  thread_ = std::thread([this, promise = std::move(promise)]() mutable {
    tid_ = currentThreadTid();
    promise.set_value(tid_);
    if (func_) {
      func_();
    }
  });
}

void Thread::join() {
  assert(started_);
  assert(!joined_);
  joined_ = true;
  thread_.join();
}

void Thread::detach() {
  assert(started_);
  assert(!joined_);
  detached_ = true;
  thread_.detach();
}

pid_t Thread::currentThreadTid() {
  return static_cast<pid_t>(::syscall(SYS_gettid));
}
} // namespace base
} // namespace Tupo