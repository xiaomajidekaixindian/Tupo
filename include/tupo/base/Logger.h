#pragma once
#include "tupo/base/LogStream.h"
namespace Tupo {
namespace base {
class Logger {
public:
  enum class LogLevel { DEBUG, INFO, WARN, ERROR };
  Logger(LogLevel level, const char *file, int line);
  ~Logger();
  LogStream &stream() { return stream_; }

  static void setOutput(void (*output)(const char *msg, int len));

private:
  LogLevel level_;
  const char *file_;
  int line_;
  LogStream stream_;
};

#define LOG_DEBUG                                                              \
  Tupo::base::Logger(Tupo::base::Logger::LogLevel::DEBUG, __FILE__, __LINE__)  \
      .stream()
#define LOG_INFO                                                               \
  Tupo::base::Logger(Tupo::base::Logger::LogLevel::INFO, __FILE__, __LINE__)   \
      .stream()
#define LOG_WARN                                                               \
  Tupo::base::Logger(Tupo::base::Logger::LogLevel::WARN, __FILE__, __LINE__)   \
      .stream()
#define LOG_ERROR                                                              \
  Tupo::base::Logger(Tupo::base::Logger::LogLevel::ERROR, __FILE__, __LINE__)  \
      .stream()

} // namespace base
} // namespace Tupo