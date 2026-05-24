#include "tupo/net/Timer.h"

namespace Tupo {
namespace net {
void Timer::restart(Tupo::base::Timestamp now) {
  if (repeat_) {
    expiration_ = Tupo::base::Timestamp::resetTime(now, interval_);
  } else {
    expiration_ = Tupo::base::Timestamp::invalid();
  }
}
} // namespace net
} // namespace Tupo