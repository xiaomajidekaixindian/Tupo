#include "tupo/base/Logger.h"
#include "tupo/base/Thread.h"
#include "tupo/net/EventLoop.h"

void threadFunc() {
  Tupo::net::EventLoop loop;
  LOG_INFO << "threadFunc(): pid:" << getpid()
           << ",tid:" << Tupo::base::Thread::currentThreadTid();
  loop.loop();
}
int main() {
  LOG_INFO << "main(): pid:" << getpid()
           << ",tid:" << Tupo::base::Thread::currentThreadTid();
  Tupo::net::EventLoop loop;

  Tupo::base::Thread thread(threadFunc);
  thread.start();

  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  loop.loop();
}