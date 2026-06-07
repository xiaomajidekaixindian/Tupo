#include "tupo/base/MutexLock.h"

namespace Tupo {
namespace base {

void MutexLock::lock() { mutex_.lock(); }

void MutexLock::unlock() { mutex_.unlock(); }

std::mutex &MutexLock::getMutex() { return mutex_; }

MutexLock::~MutexLock() {
  // 确保在析构时没有线程持有锁
  if (mutex_.try_lock()) {
    mutex_.unlock();
  }
}

MutexLockGuard::MutexLockGuard(MutexLock &mutex) : mutex_(mutex) {
  mutex_.lock();
}

MutexLockGuard::~MutexLockGuard() { mutex_.unlock(); }
} // namespace base
} // namespace Tupo