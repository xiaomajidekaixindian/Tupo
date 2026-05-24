#include "tupo/base/Thread.h"
#include <iostream>

namespace Tupo {
namespace base {

Thread::Thread(ThreadFunc func, const std::string &name)
    : started_(false), joined_(false), tid_(0), func_(std::move(func)) {}
Thread::~Thread() {
  if (started_ && !joined_ && thread_.joinable()) {
    thread_.detach();
  }
}
void Thread::start() {
  assert(!started_);
  started_ = true;

  // 使用过lambda捕获this
  thread_ = std::thread([this]() {
    tid_ = currentThreadTid();
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

pid_t Thread::currentThreadTid() {
  return static_cast<pid_t>(::syscall(SYS_gettid));
}
} // namespace base
} // namespace Tupo