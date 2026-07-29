# 指定最低编译版本
cmake_minimum_required(VERSION 3.14)
project(Tupo)

# 设置C++17标准
set(CMAKE_CXX_STANDARD 17)

# 设置全局包含路径
include_directories(${CMAKE_SOURCE_DIR}/include)

# 创建基础库 - 显式列出所有源文件
add_library(tupo_base
    src/base/MutexLock.cpp
    src/base/Condition.cpp   
    src/base/Thread.cpp
    src/base/Timestamp.cpp
)

# 创建网络库    
add_library(tupo_net
    src/net/EventLoop.cpp
    src/net/poller/PollPoller.cpp
    src/net/Channel.cpp
    src/net/Poller.cpp
    src/net/poller/EpollPoller.cpp
    src/net/Timer.cpp
    src/net/TimerQueue.cpp
    src/net/Socket.cpp  
    src/net/InetAddress.cpp
    src/net/Acceptor.cpp
    src/net/TcpConnection.cpp
    src/net/TcpServer.cpp
)
# 如果网络库依赖基础库，添加链接
target_link_libraries(tupo_net PUBLIC tupo_base)
# 添加测试目录
# add_subdirectory(tests)