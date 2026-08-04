cmake_minimum_required(VERSION 3.10)
project(tupo_tests)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 使用绝对路径
get_filename_component(TUPO_INCLUDE_DIR "../include" ABSOLUTE)
get_filename_component(TUPO_BUILD_DIR "../build" ABSOLUTE)

# 检查必要文件和目录
foreach(CHECK_PATH 
    ${TUPO_INCLUDE_DIR}
    ${TUPO_BUILD_DIR}/libtupo_base.a
    ${TUPO_BUILD_DIR}/libtupo_net.a
)
    if(NOT EXISTS ${CHECK_PATH})
        message(FATAL_ERROR "Required path not found: ${CHECK_PATH}")
    endif()
endforeach()

message(STATUS "Using include directory: ${TUPO_INCLUDE_DIR}")

# 查找必要的包
find_package(Threads REQUIRED)
find_package(GTest REQUIRED)

# 创建导入目标 - 使用绝对路径
add_library(tupo_base STATIC IMPORTED GLOBAL)
set_target_properties(tupo_base PROPERTIES
    IMPORTED_LOCATION ${TUPO_BUILD_DIR}/libtupo_base.a
    INTERFACE_INCLUDE_DIRECTORIES "${TUPO_INCLUDE_DIR}"  # 使用绝对路径
)

add_library(tupo_net STATIC IMPORTED GLOBAL)
set_target_properties(tupo_net PROPERTIES
    IMPORTED_LOCATION ${TUPO_BUILD_DIR}/libtupo_net.a
    INTERFACE_INCLUDE_DIRECTORIES "${TUPO_INCLUDE_DIR}"  # 使用绝对路径
    INTERFACE_LINK_LIBRARIES tupo_base
)

# MutexLock 测试
add_executable(mutexlock_test base/MutexLock_test.cpp)
target_link_libraries(mutexlock_test tupo_base GTest::gtest GTest::gtest_main Threads::Threads)

# EventLoop 测试
add_executable(eventloop_test net/EventLoop_test.cpp)
target_link_libraries(eventloop_test tupo_net GTest::gtest GTest::gtest_main Threads::Threads)

# PollPoller测试
add_executable(pollpoller_test net/PollPoller_test.cpp)
target_link_libraries(pollpoller_test tupo_net GTest::gtest GTest::gtest_main Threads::Threads)

# EPollPoller测试
add_executable(epollpoller_test net/EpollPoller_test.cpp)
target_link_libraries(epollpoller_test tupo_net GTest::gtest GTest::gtest_main Threads::Threads)

# Timestamp测试 - 根据实际情况调整路径
if(EXISTS base/Timestamp_test.cpp)
    add_executable(timestamp_test base/Timestamp_test.cpp)
    target_link_libraries(timestamp_test tupo_base GTest::gtest GTest::gtest_main Threads::Threads)
elseif(EXISTS net/Timestamp_test.cpp)
    add_executable(timestamp_test net/Timestamp_test.cpp)
    target_link_libraries(timestamp_test tupo_base GTest::gtest GTest::gtest_main Threads::Threads)
else()
    message(WARNING "Timestamp_test.cpp not found, skipping timestamp_test")
endif()

# Timer测试
add_executable(timer_test net/Timer_test.cpp)
target_link_libraries(timer_test tupo_base tupo_net GTest::gtest GTest::gtest_main Threads::Threads)


# TimerQueue测试
add_executable(timerqueue_test net/TimerQueue_test.cpp)
target_link_libraries(timerqueue_test tupo_base tupo_net GTest::gtest GTest::gtest_main Threads::Threads)

# TimerId测试
add_executable(timerid_test net/TimerId_test.cpp)
target_link_libraries(timerid_test tupo_base tupo_net GTest::gtest GTest::gtest_main Threads::Threads)

# TimerQueueMultiThread测试
add_executable(TimerQueueMultiThreadTest net/TimerQueueMultiThreadTest.cpp)  
target_link_libraries(TimerQueueMultiThreadTest tupo_base tupo_net GTest::gtest GTest::gtest_main Threads::Threads)

# Thread测试
add_executable(Thread_test base/Thread_test.cpp)
target_link_libraries(Thread_test tupo_base GTest::gtest GTest::gtest_main Threads::Threads)

# EventLoop单元测试
add_executable(EventLoop_UnitTest net/EventLoop_UnitTest.cpp)
target_link_libraries(EventLoop_UnitTest tupo_net GTest::gtest GTest::gtest_main Threads::Threads)  

# Socket单元测试
add_executable(Socket_test net/Socket_test.cpp)
target_link_libraries(Socket_test tupo_net GTest::gtest GTest::gtest_main Threads::Threads)  

# InetAddress单元测试
add_executable(InetAddress_test net/InetAddress_test.cpp)
target_link_libraries(InetAddress_test tupo_net GTest::gtest GTest::gtest_main Threads::Threads)

# TcpServer单元测试
add_executable(TcpServer_test net/TcpServer_test.cpp)
target_link_libraries(TcpServer_test tupo_net GTest::gtest GTest::gtest_main Threads::Threads)

# Buffer单元测试
add_executable(Buffer_test net/Buffer_test.cpp)
target_link_libraries(Buffer_test tupo_net GTest::gtest GTest::gtest_main Threads::Threads)