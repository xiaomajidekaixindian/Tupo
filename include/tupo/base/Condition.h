#pragma

#include "tupo/base/MutexLock.h"
#include <condition_variable>

namespace Tupo {
namespace base {
class Condition {

public:
  explicit Condition(MutexLock &mutex)
      : mutex_(mutex) {

        };

  void notify();
  void notiayAll();
  void wait();

  Condition(const Condition &) = delete;
  Condition &operator=(const Condition &) = delete;
  Condition(const Condition &&) = delete;
  Condition &&operator=(const Condition &&) = delete;

private:
  std::condition_variable cond_;
  MutexLock &mutex_;
};
} // namespace base
} // namespace Tupo