#include "tupo/base/Logger.h"
#include "tupo/base/Thread.h"
#include "tupo/base/Timestamp.h"

namespace Tupo {
namespace base {
namespace {
void defaultOutput(const char *msg, int len) { fwrite(msg, 1, len, stdout); }

void (*g_output)(const char *msg, int len) = defaultOutput;
} // namespace

Logger::Logger(LogLevel level, const char *file, int line)
    : level_(level), file_(file), line_(line) {
  stream_ << "[" << Timestamp::now().toFormattedString() << "] " << "["
          << Tupo::base::Thread::currentThreadTid() << "] ";
  switch (level_) {
  case LogLevel::DEBUG:
    stream_ << "[DEBUG] ";
    break;
  case LogLevel::INFO:
    stream_ << "[INFO] ";
    break;
  case LogLevel::WARN:
    stream_ << "[WARN] ";
    break;
  case LogLevel::ERROR:
    stream_ << "[ERROR] ";
    break;
  }
}

void Logger::setOutput(void (*output)(const char *msg, int len)) {
  g_output = output;
}

Logger::~Logger() {
  stream_ << " [" << file_ << ":" << line_ << "]\n";
  g_output(stream_.data(), stream_.size());
}
} // namespace base
} // namespace Tupo