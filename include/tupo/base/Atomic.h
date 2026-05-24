
#pragma once

#include <atomic>
namespace Tupo {
namespace base {
class AtomicInt32 {
public:
  AtomicInt32(int32_t value = 0) : value_(value) {}
  int32_t get() { return value_.load(std::memory_order_relaxed); }
  
private:
  std::atomic<int32_t> value_;
};
} // namespace base
} // namespace Tupo