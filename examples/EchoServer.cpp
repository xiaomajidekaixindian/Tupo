#include "tupo/net/TcpServer.h"
#include "tupo/net/EventLoop.h"
#include "tupo/net/Buffer.h"
#include "tupo/net/TcpConnection.h"
#include "tupo/net/InetAddress.h"
#include <csignal>
#include <iostream>
class EchoServer { 
public:
    EchoServer(Tupo::net::EventLoop *loop, const Tupo::net::InetAddress &addr)
        : loop_(loop), server_(std::make_unique<Tupo::net::TcpServer>(loop, addr)) {
        server_->setConnectionCallback([this](const Tupo::net::TcpConnection::TcpConnectionPtr &conn) {
            this->onConnection(conn);
        });
        server_->setMessageCallback([this](const Tupo::net::TcpConnection::TcpConnectionPtr &conn, Tupo::net::Buffer &buffer) {
            this->onMessage(conn, buffer);
        });
        server_->setWriteCompleteCallback([this](const Tupo::net::TcpConnection::TcpConnectionPtr &conn) {
            this->onWriteComplete(conn);
        });
    }

    void start() {
        server_->start();
    } 

    void stop() {
        loop_->quit();  
    }
private:
    void onConnection(const Tupo::net::TcpConnection::TcpConnectionPtr conn){
        if(conn->getState() == Tupo::net::TcpConnection::kConnected){
            std::cout << "新连接: " << conn->getPeerAddress().toIpPort() << std::endl;
        } else if(conn->getState() == Tupo::net::TcpConnection::kDisconnected){
            std::cout << "连接断开: " << conn->getPeerAddress().toIpPort() << std::endl;
        }
    }


    void onMessage(const Tupo::net::TcpConnection::TcpConnectionPtr conn, Tupo::net::Buffer& buffer) {
    // 1. 把收到的数据拿出来
    std::string request = buffer.retrieveAllAsString();
    
    // 2. 检查是不是 HTTP 请求（只处理 GET /）
    if (request.find("GET / ") != std::string::npos || 
        request.find("GET / HTTP") != std::string::npos) {
        
        // 3. 构造 HTTP 响应
        std::string response = 
            "HTTP/1.1 200 OK\r\n"
            "Content-Length: 13\r\n"
            "Connection: keep-alive\r\n"
            "\r\n"
            "Hello, World!";
        
        conn->send(response);
    } else {
        // 其他请求直接回显（保持 echo 功能）
        conn->send(request);
    }
}

    void onWriteComplete(const Tupo::net::TcpConnection::TcpConnectionPtr conn) {
        std::cout << "[发送完成] " << conn->getPeerAddress().toIpPort() 
                  << std::endl;
    }
    
    void onClose(const Tupo::net::TcpConnection::TcpConnectionPtr conn) {
        std::cout << "[关闭] " << conn->getPeerAddress().toIpPort() 
                  << " 连接已关闭" << std::endl;
    }
    Tupo::net::EventLoop *loop_;
    std::unique_ptr<Tupo::net::TcpServer> server_;
};

std::atomic<bool> g_running{true};
EchoServer *g_echoServer = nullptr;

void signalHandler(int signal) {
    if (signal == SIGINT || signal == SIGTERM) {
        std::cout << "\n收到停止信号，正在关闭服务器..." << std::endl;
        g_running = false;
        if (g_echoServer) {
            g_echoServer->stop(); // 停止服务器
        }
    }
}   

int main(int argc,char *argv[]){
    // 默认端口
    uint16_t port = 8080;
    
    // 解析命令行参数
    if (argc > 1) {
        port = static_cast<uint16_t>(std::stoi(argv[1]));
    }

    // 注册信号处理
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    std::cout << "=== Tupo Echo Server ===" << std::endl;
    std::cout << "端口: " << port << std::endl;
    std::cout << "按 Ctrl+C 停止服务器" << std::endl;
    std::cout << "=========================" << std::endl;

    Tupo::net::EventLoop loop;
    Tupo::net::InetAddress listenAddr(port);
    EchoServer echoServer(&loop,listenAddr);    
    g_echoServer = &echoServer;
    try {
        echoServer.start();
        loop.loop();  // 进入事件循环
    } catch (const std::exception& e) {
        std::cerr << "错误: " << e.what() << std::endl;
        return 1;
    }
}