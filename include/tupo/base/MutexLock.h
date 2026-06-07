
#pragma once
#include <mutex>

namespace Tupo {
namespace base {

class MutexLock {
public:
  MutexLock() = default;
  MutexLock(const MutexLock &) = delete;
  MutexLock &operator=(const MutexLock &) = delete;
  MutexLock(const MutexLock &&) = delete;
  MutexLock &&operator=(const MutexLock &&) = delete;
  ~MutexLock();

  void lock();
  void unlock();
  std::mutex &getMutex();

private:
  std::mutex mutex_;
};

// RAII包装器

class MutexLockGuard {
public:
  explicit MutexLockGuard(MutexLock &mutex);

  ~MutexLockGuard();
  MutexLockGuard(const MutexLockGuard &) = delete;
  MutexLockGuard &operator=(const MutexLockGuard &) = delete;
  MutexLockGuard(MutexLockGuard &&) = delete;
  MutexLockGuard &operator=(MutexLockGuard &&) = delete;

private:
  MutexLock &mutex_;
};

} // namespace base
} // namespace Tupo