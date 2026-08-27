# CMakeLists.txt
# 指定最低编译版本
cmake_minimum_required(VERSION 3.14)
project(Tupo)

option(ENABLE_DEBUG "Enable debug mode for test functions" OFF)

# ========== 构建类型设置 ==========
# 默认使用 Release 模式
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
    message(STATUS "📦 Default build type: Release")
endif()

message(STATUS "🔧 Build type: ${CMAKE_BUILD_TYPE}")

# ========== DEBUG 宏控制 ==========
option(ENABLE_DEBUG "Enable debug mode for test functions" OFF)

if(ENABLE_DEBUG)
    add_compile_definitions(DEBUG)
    # Debug 模式下，设置 CMAKE_BUILD_TYPE 为 Debug
    if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type" FORCE)
        message(STATUS "🔧 Debug mode: ENABLED (switched to Debug build)")
    endif()
    message(STATUS "🔧 Test functions: VISIBLE")
else()
    # Release 模式下，确保没有 DEBUG 宏
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
        message(STATUS "🚀 Debug mode: DISABLED (switched to Release build)")
    endif()
    message(STATUS "🚀 Test functions: HIDDEN")
endif()

option(BUILD_HTTP_TESTS "Build HTTP tests" OFF)  # ✅ 在这里定义


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
    src/net/Buffer.cpp
)
# 如果网络库依赖基础库，添加链接
target_link_libraries(tupo_net PUBLIC tupo_base)

# 添加测试目录
# add_subdirectory(tests)

# 引入HTTP子模块
add_subdirectory(src/net/http)
