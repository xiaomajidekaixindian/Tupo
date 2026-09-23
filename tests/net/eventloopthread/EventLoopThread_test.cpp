#include "tupo/net/EventLoopThread.h"
#include "tupo/net/TcpServer.h"
#include <iostream>
class EventLoopThreadTest {
public:
  EventLoopThreadTest(Tupo::net::EventLoop *loop) : loop_(loop) {
    server_.setConnectionCallback(
        [this](const Tupo::net::TcpConnection::TcpConnectionPtr &conn) {
          this->onConnected();
        });
  }

  void start() { server_.start(); }

private:
  void onConnected() { std::cout << "连接成功" << std::endl; }

  Tupo::net::EventLoop *loop_;
  Tupo::net::InetAddress addr_{8080};
  Tupo::net::TcpServer server_{loop_, addr_};
};

int main() {
  std::cout << "caller thread: " << std::this_thread::get_id() << std::endl;

  Tupo::net::EventLoopThread thread;
  Tupo::net::EventLoop *loop = thread.startLoop();

  std::cout << "got loop: " << loop << std::endl;

  // 在这个 EventLoop 上跑一个任务
  loop->runInLoop([&]() {
    std::cout << "task running in thread: " << std::this_thread::get_id()
              << std::endl;
  });

  // 等任务执行完
  std::this_thread::sleep_for(std::chrono::seconds(1));

  // 退出事件循环
  loop->quit();

  return 0;
}