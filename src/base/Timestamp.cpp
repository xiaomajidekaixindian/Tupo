#include "tupo/base/Timestamp.h"
#include <cstddef>
#include <sys/time.h>
namespace Tupo {
namespace base {

Timestamp Timestamp::now() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return Timestamp(tv.tv_sec * kMicroSecondsPerSecond + tv.tv_usec);
}

// 获取到期时间
Timestamp Timestamp::resetTime(Timestamp now, double interval) {
  int64_t delay =
      static_cast<int64_t>(interval * Timestamp::kMicroSecondsPerSecond);
  return Timestamp(delay + now.microSecondsSinceEpoch());
}

} // namespace base
} // namespace Tupo