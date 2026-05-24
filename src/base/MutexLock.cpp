#include "tupo/base/MutexLock.h"

namespace Tupo {
namespace base {

void MutexLock::lock() { mutex_.lock(); }

void MutexLock::unlock() { mutex_.unlock(); }

std::mutex &MutexLock::getMutex() { return mutex_; }

MutexLockGuard::MutexLockGuard(MutexLock &mutex) : mutex_(mutex) {
  mutex_.lock();
}

MutexLockGuard::~MutexLockGuard() { mutex_.unlock(); }
} // namespace base
} // namespace Tupo