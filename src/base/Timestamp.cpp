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

std::string Timestamp::toFormattedString(bool showMicroseconds) const {
  char buf[64] = {0};
  time_t seconds = static_cast<time_t>(microSecondsSinceEpoch_ /
                                       Timestamp::kMicroSecondsPerSecond);
  struct tm tm_time;
  localtime_r(&seconds, &tm_time);

  if (showMicroseconds) {
    int microseconds = static_cast<int>(microSecondsSinceEpoch_ %
                                        Timestamp::kMicroSecondsPerSecond);
    snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d.%06d",
             tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
             tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec, microseconds);
  } else {
    snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d",
             tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
             tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
  }
  return buf;
}
} // namespace base
} // namespace Tupo