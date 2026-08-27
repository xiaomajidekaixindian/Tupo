#!/bin/bash

# 自动退出脚本，如果任何一条命令执行失败
set -euo pipefail

# 颜色输出
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# ========== 获取项目根目录 ==========
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"  # tests 的上级目录

echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN}  Tupo 项目构建脚本${NC}"
echo -e "${GREEN}  项目根目录: ${PROJECT_ROOT}${NC}"
echo -e "${GREEN}========================================${NC}"

# ========== 解析命令行参数 ==========
DEBUG_MODE="OFF"
CLEAN_BUILD="OFF"

while [[ $# -gt 0 ]]; do
    case $1 in
        --debug|-d)
            DEBUG_MODE="ON"
            shift
            ;;
        --release|-r)
            DEBUG_MODE="OFF"
            shift
            ;;
        --clean|-c)
            CLEAN_BUILD="ON"
            shift
            ;;
        --help|-h)
            cat << EOF
用法: $0 [选项]

选项:
  --debug, -d    开启 DEBUG 模式（测试函数可见）
  --release, -r  关闭 DEBUG 模式（默认）
  --clean, -c    强制清理 build 目录
  --help, -h     显示帮助信息

示例:
  $0              # 默认 Release 模式构建
  $0 --debug      # Debug 模式构建
  $0 -d -c        # Debug 模式 + 清理重新构建
EOF
            exit 0
            ;;
        *)
            echo -e "${RED}未知参数: $1${NC}"
            exit 1
            ;;
    esac
done

echo -e "  DEBUG 模式: $([ "$DEBUG_MODE" == "ON" ] && echo -e "${YELLOW}开启${NC}" || echo -e "${YELLOW}关闭${NC}")"
echo -e "  清理构建:   $([ "$CLEAN_BUILD" == "ON" ] && echo -e "${YELLOW}是${NC}" || echo -e "${YELLOW}否${NC}")"
echo -e "${GREEN}========================================${NC}"

# ========== 构建主库 ==========
echo -e "\n${GREEN}[1/3] 构建主库...${NC}"

# 主库构建目录
MAIN_BUILD_DIR="${PROJECT_ROOT}/build"

if [ "$CLEAN_BUILD" == "ON" ] && [ -d "$MAIN_BUILD_DIR" ]; then
    echo -e "${YELLOW}清理主库构建目录...${NC}"
    rm -rf "$MAIN_BUILD_DIR"
fi

if [ ! -d "$MAIN_BUILD_DIR" ]; then
    mkdir -p "$MAIN_BUILD_DIR"
fi

# 进入主库构建目录
cd "$MAIN_BUILD_DIR"

# 生成主库构建文件
if [ "$DEBUG_MODE" == "ON" ]; then
    cmake "$PROJECT_ROOT" -DENABLE_DEBUG=ON
else
    cmake "$PROJECT_ROOT" -DENABLE_DEBUG=OFF
fi

# 编译主库
cmake --build "$MAIN_BUILD_DIR" -j$(nproc 2>/dev/null || echo 4)

# ========== 构建测试 ==========
echo -e "\n${GREEN}[2/3] 构建测试...${NC}"

# 测试构建目录
TEST_BUILD_DIR="${SCRIPT_DIR}/build"

if [ "$CLEAN_BUILD" == "ON" ] && [ -d "$TEST_BUILD_DIR" ]; then
    echo -e "${YELLOW}清理测试构建目录...${NC}"
    rm -rf "$TEST_BUILD_DIR"
fi

if [ ! -d "$TEST_BUILD_DIR" ]; then
    mkdir -p "$TEST_BUILD_DIR"
fi

# 进入测试构建目录
cd "$TEST_BUILD_DIR"

# 生成测试构建文件
if [ "$DEBUG_MODE" == "ON" ]; then
    cmake "$SCRIPT_DIR" -DENABLE_DEBUG=ON
else
    cmake "$SCRIPT_DIR" -DENABLE_DEBUG=OFF
fi

# 编译测试
cmake --build "$TEST_BUILD_DIR" -j$(nproc 2>/dev/null || echo 4)

# ========== 完成 ==========
echo -e "\n${GREEN}[3/3] 构建完成！${NC}"
echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN} ✅ 主库位置: ${MAIN_BUILD_DIR}/libtupo_*.a${NC}"
echo -e "${GREEN} ✅ 测试位置: ${TEST_BUILD_DIR}/*_test${NC}"

echo -e "${GREEN}========================================${NC}"