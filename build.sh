#!/bin/bash

# 自动退出脚本，如果任何一条命令执行失败
set -euo pipefail

# 颜色输出
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

# ========== 默认配置 ==========
BUILD_TYPE="Release"
DEBUG_MODE="OFF"
CLEAN_BUILD="OFF"
BUILD_TESTS="OFF"
JOBS=$(nproc 2>/dev/null || echo 4)

# ========== 显示帮助 ==========
show_help() {
    echo "用法: $0 [选项]"
    echo ""
    echo "选项:"
    echo "  --debug, -d        开启 DEBUG 模式 (CMake Build Type: Debug)"
    echo "  --release, -r      使用 Release 模式（默认）"
    echo "  --clean, -c        强制清理 build 目录"
    echo "  --test, -t         编译测试"
    echo "  --help, -h         显示帮助"
    echo ""
    echo "示例:"
    echo "  $0                  # Release 构建"
    echo "  $0 --debug          # Debug 构建"
    echo "  $0 --debug --test   # Debug 构建 + 测试"
    echo "  $0 --clean          # 清理后构建"
    exit 0
}

# ========== 解析命令行参数 ==========
while [[ $# -gt 0 ]]; do
    case $1 in
        --debug|-d)
            BUILD_TYPE="Debug"
            DEBUG_MODE="ON"
            shift
            ;;
        --release|-r)
            BUILD_TYPE="Release"
            DEBUG_MODE="OFF"
            shift
            ;;
        --clean|-c)
            CLEAN_BUILD="ON"
            shift
            ;;
        --test|-t)
            BUILD_TESTS="ON"
            shift
            ;;
        --help|-h)
            show_help
            ;;
        *)
            echo -e "${RED}未知参数: $1${NC}"
            show_help
            ;;
    esac
done

# ========== 显示配置 ==========
echo -e "${GREEN}========================================${NC}"
echo -e "${BLUE}  ⚙️  构建配置${NC}"
echo -e "${GREEN}========================================${NC}"
echo -e "  📦 构建类型:   ${YELLOW}${BUILD_TYPE}${NC}"
echo -e "  🔧 DEBUG 模式: ${YELLOW}${DEBUG_MODE}${NC}"
echo -e "  🧪 测试:       ${YELLOW}${BUILD_TESTS}${NC}"
echo -e "  💻 CPU 核心:   ${YELLOW}${JOBS}${NC}"
echo -e "${GREEN}========================================${NC}"

# ========== 清理构建目录 ==========
if [ "${CLEAN_BUILD}" == "ON" ] || [ ! -d "build" ]; then
    echo -e "\n${GREEN}🧹 清理旧构建目录...${NC}"
    rm -rf build
    echo -e "${GREEN}✅ 清理完成${NC}"
fi

# ========== 创建构建目录 ==========
mkdir -p build

# ========== 构建 ==========
echo -e "\n${GREEN}[1/3] 生成构建文件...${NC}"
cd build

# 生成构建文件
cmake .. \
    -DCMAKE_BUILD_TYPE=${BUILD_TYPE} \
    -DENABLE_DEBUG=${DEBUG_MODE} \
    -DBUILD_HTTP_TESTS=${BUILD_TESTS}

echo -e "\n${GREEN}[2/3] 开始编译 (使用 ${JOBS} 个核心)...${NC}"
cmake --build . -j ${JOBS}

echo -e "\n${GREEN}[3/3] 检查生成的文件...${NC}"
if [ -f "libtupo_base.a" ]; then
    echo -e "  ✅ libtupo_base.a"
fi
if [ -f "libtupo_net.a" ]; then
    echo -e "  ✅ libtupo_net.a"
fi
if [ -f "src/net/http/libtupo_http.a" ]; then
    echo -e "  ✅ libtupo_http.a"
fi

echo -e "\n${GREEN}========================================${NC}"
echo -e "${GREEN} ✅ 构建完成！${NC}"
echo -e "${GREEN}========================================${NC}"
echo -e "📂 构建目录: ${YELLOW}build/${NC}"

# 如果测试被编译，提示运行测试
if [ "${BUILD_TESTS}" == "ON" ]; then
    echo -e "\n🧪 运行测试: ${YELLOW}cd build && ctest --output-on-failure${NC}"
fi
