#include "tupo/base/Logger.h"
#include "tupo/net/Buffer.h"
#include "tupo/net/EventLoop.h"
#include "tupo/net/InetAddress.h"
#include "tupo/net/TcpConnection.h"
#include "tupo/net/TcpServer.h"
#include "tupo/net/http/HttpContext.h"
#include "tupo/net/http/HttpRequest.h"
#include "tupo/net/http/HttpResponse.h"
#include <csignal>
class EchoServer {
public:
  EchoServer(Tupo::net::EventLoop *loop, const Tupo::net::InetAddress &addr,
             int threadNums = 0)
      : loop_(loop), server_(std::make_unique<Tupo::net::TcpServer>(
                         loop, addr, threadNums)) {
    server_->setConnectionCallback(
        [this](const Tupo::net::TcpConnection::TcpConnectionPtr &conn) {
          this->onConnection(conn);
        });
    server_->setMessageCallback(
        [this](const Tupo::net::TcpConnection::TcpConnectionPtr &conn,
               Tupo::net::Buffer &buffer) { this->onMessage(conn, buffer); });
    server_->setWriteCompleteCallback(
        [this](const Tupo::net::TcpConnection::TcpConnectionPtr &conn) {
          this->onWriteComplete(conn);
        });
  }

  void start() { server_->start(); }

  void stop() { loop_->quit(); }

private:
  void onConnection(const Tupo::net::TcpConnection::TcpConnectionPtr conn) {
    if (conn->getState() == Tupo::net::TcpConnection::kConnected) {
      LOG_INFO << "新连接: " << conn->getPeerAddress().toIpPort();
    } else if (conn->getState() == Tupo::net::TcpConnection::kDisconnected) {
      LOG_INFO << "连接关闭: " << conn->getPeerAddress().toIpPort();
    }
  }

  void onMessage(const Tupo::net::TcpConnection::TcpConnectionPtr conn,
                 Tupo::net::Buffer &buffer) {
    // 1. 把收到的数据拿出来
    std::string request = buffer.retrieveAllAsString();
    Tupo::net::HttpRequest httpRequest;
    Tupo::net::HttpContext httpContext;
    if (httpContext.ParseRequest(&buffer, &httpRequest)) {
      std::string response = "HTTP/1.1 200 OK\r\n"
                             "Content-Length: 13\r\n"
                             "Connection: keep-alive\r\n"
                             "\r\n"
                             "Hello, World!";

      conn->send(response);
    } else {
      conn->send(request);
    }
  }

  void onWriteComplete(const Tupo::net::TcpConnection::TcpConnectionPtr conn) {
    LOG_INFO << "[发送完成] " << conn->getPeerAddress().toIpPort();
  }

  void onClose(const Tupo::net::TcpConnection::TcpConnectionPtr conn) {
    LOG_INFO << "[关闭] " << conn->getPeerAddress().toIpPort() << " 连接已关闭";
  }
  Tupo::net::EventLoop *loop_;
  std::unique_ptr<Tupo::net::TcpServer> server_;
};

std::atomic<bool> g_running{true};
EchoServer *g_echoServer = nullptr;

void signalHandler(int signal) {
  if (signal == SIGINT || signal == SIGTERM) {
    LOG_INFO << "收到停止信号，正在关闭服务器...";
    g_running = false;
    if (g_echoServer) {
      g_echoServer->stop(); // 停止服务器
    }
  }
}

int main(int argc, char *argv[]) {
  // 默认端口
  uint16_t port = 8080;

  // 解析命令行参数
  if (argc > 1) {
    port = static_cast<uint16_t>(std::stoi(argv[1]));
  }

  // 注册信号处理
  signal(SIGINT, signalHandler);
  signal(SIGTERM, signalHandler);

  LOG_INFO << "=== Tupo Echo Server ===";
  LOG_INFO << "端口: " << port;
  LOG_INFO << "按 Ctrl+C 停止服务器";
  LOG_INFO << "=========================";

  Tupo::net::EventLoop loop;
  Tupo::net::InetAddress listenAddr(port);
  EchoServer echoServer(&loop, listenAddr, 4);
  g_echoServer = &echoServer;
  try {
    echoServer.start();
    loop.loop(); // 进入事件循环
  } catch (const std::exception &e) {
    LOG_ERROR << "错误: " << e.what();
    return 1;
  }
}