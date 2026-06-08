#include "tupo/base/Thread.h"
#include <future>
#include <iostream>

namespace Tupo {
namespace base {

Thread::Thread(ThreadFunc func, const std::string &name)
    : started_(false), joined_(false), tid_(0), func_(std::move(func)),
      detached_(false), name_(name) {}
Thread::~Thread() {
  if (started_ && !joined_ && thread_.joinable() && !detached_) {
    thread_.detach();
  }
}

// 移动构造函数
Thread::Thread(Thread &&other) noexcept
    : started_(other.started_.load()), joined_(other.joined_.load()),
      tid_(other.tid_.load()), func_(std::move(other.func_)),
      detached_(other.detached_.load()), name_(std::move(other.name_)) {
  other.started_ = false;
  other.joined_ = false;
  other.tid_ = 0;
  other.detached_ = false;
}

// 移动赋值运算符
Thread &Thread::operator=(Thread &&other) noexcept {
  if (this != &other) {
    if (started_ && !joined_ && thread_.joinable() && !detached_) {
      thread_.detach();
    }
    started_ = other.started_.load();
    joined_ = other.joined_.load();
    tid_ = other.tid_.load();
    func_ = std::move(other.func_);
    detached_ = other.detached_.load();
    name_ = std::move(other.name_);

    other.started_ = false;
    other.joined_ = false;
    other.tid_ = 0;
    other.detached_ = false;
  }
  return *this;
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
  pid_t tid = future.get();
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


// 获取的是当前线程的tid（调用这个函数的线程ID），而不是Thread对象所代表的线程的tid
pid_t Thread::currentThreadTid() {
  return static_cast<pid_t>(::syscall(SYS_gettid));
}
} // namespace base
} // namespace Tupo