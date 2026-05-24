#pragma

#include "tupo/base/Condition.h"
#include "tupo/base/MutexLock.h"

namespace Tupo {
namespace base {
void Condition::notify() { cond_.notify_one(); }

void Condition::notiayAll() { cond_.notify_all(); }

void Condition::wait() {
  std::unique_lock<std::mutex> lock(mutex_.getMutex(), std::adopt_lock);
  cond_.wait(lock);
  // unique_lock 在析构时会自动管理锁，我们通过 release 避免重复解锁
  lock.release();
}
} // namespace base
} // namespace Tupo