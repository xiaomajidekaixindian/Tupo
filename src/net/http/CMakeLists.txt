# src/net/http/CMakeLists.txt
cmake_minimum_required(VERSION 3.14)

# 收集 HTTP 模块的源文件
set(HTTP_SOURCES
    HttpRequest.cpp
    HttpResponse.cpp
)

# 创建 HTTP 子库
add_library(tupo_http ${HTTP_SOURCES})

# 指定头文件路径（让依赖者能找到 include/tupo/net/http/xxx.h）
target_include_directories(tupo_http PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

# 链接依赖：HTTP 模块依赖网络库和基础库
target_link_libraries(tupo_http
    PUBLIC tupo_net
    PRIVATE tupo_base
)