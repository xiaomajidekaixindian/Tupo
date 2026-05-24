#include "tupo/base/Thread.h"
#include "tupo/net/EventLoop.h"
#include <iostream>

void threadFunc() {
  Tupo::net::EventLoop loop;
  std::cout << "threadFunc(): pid:" << getpid()
            << ",tid:" << Tupo::base::Thread::currentThreadTid() << std::endl;
  loop.loop();
}
int main() {
  std::cout << "main(): pid:" << getpid()
            << ",tid:" << Tupo::base::Thread::currentThreadTid() << std::endl;
  Tupo::net::EventLoop loop;

  Tupo::base::Thread thread(threadFunc);
  thread.start();

  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  loop.loop();
}