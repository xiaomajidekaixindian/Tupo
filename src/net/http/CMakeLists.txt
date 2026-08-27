# src/net/http/CMakeLists.txt

# ========== HTTP 库 ==========
add_library(tupo_http
    HttpContext.cpp
    HttpRequest.cpp
    HttpResponse.cpp
)

# 链接网络库
target_link_libraries(tupo_http
    PUBLIC tupo_net
    PRIVATE tupo_base
)

# 设置包含路径
target_include_directories(tupo_http
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src/net/http
)

# ========== 编译选项 ==========
# 根据构建类型设置不同的编译选项
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(tupo_http PRIVATE -g -O0)
else()
    target_compile_options(tupo_http PRIVATE -O3)
endif()
