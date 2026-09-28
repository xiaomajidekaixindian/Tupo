#include "tupo/net/EventLoopThread.h"
#include "tupo/net/TcpServer.h"
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
  void onConnected() { LOG_INFO << "连接成功"; }

  Tupo::net::EventLoop *loop_;
  Tupo::net::InetAddress addr_{8080};
  Tupo::net::TcpServer server_{loop_, addr_};
};

void outName() {
  LOG_DEBUG << "task running in thread: "
            << Tupo::base::Thread::currentThreadTid();
}

int main() {
  LOG_DEBUG << "caller thread: " << Tupo::base::Thread::currentThreadTid();

  Tupo::net::EventLoopThread thread;
  Tupo::net::EventLoop *loop = thread.startLoop();

  LOG_DEBUG << "got loop: " << loop;

  // 在这个 EventLoop 上跑一个任务
  loop->runInLoop([&]() { outName(); });

  // 等任务执行完
  std::this_thread::sleep_for(std::chrono::seconds(1));

  // 退出事件循环
  loop->quit();

  return 0;
}