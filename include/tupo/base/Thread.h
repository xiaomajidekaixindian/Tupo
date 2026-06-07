#pragma once
#include <assert.h>
#include <atomic>
#include <functional>
#include <string>
#include <sys/syscall.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>
namespace Tupo {
namespace base {
class Thread {
public:
  using ThreadFunc = std::function<void()>;
  explicit Thread(ThreadFunc func, const std::string &name = std::string());
  ~Thread();
  Thread(const Thread &) = delete;
  Thread &operator=(const Thread &) = delete;
  Thread(Thread &&other) noexcept;
  Thread &operator=(Thread &&other) noexcept;

  void start();
  void join();
  void detach();

  bool started() { return started_; }
  bool joined() { return joined_; }
  bool detached() { return detached_; }
  static pid_t currentThreadTid(); // 获取当前线程id

private:
  std::atomic<bool> started_;
  std::atomic<bool> joined_;
  std::atomic<bool> detached_;
  std::atomic<pid_t> tid_;
  std::thread thread_;
  ThreadFunc func_;
};
} // namespace base
} // namespace Tupo