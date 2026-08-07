#include "tupo/net/TcpServer.h"
#include "tupo/net/EventLoop.h"
int main(){
    Tupo::net::InetAddress listenAddr(8080);
    Tupo::net::EventLoop loop;
    Tupo::net::TcpServer server(&loop,listenAddr);
    server.start();
    loop.loop();
}