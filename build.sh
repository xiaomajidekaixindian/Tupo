#!/bin/bash

# 自动退出脚本，如果任何一条命令执行失败
set -euo pipefail

# 颜色输出
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

# ========== 解析命令行参数 ==========
DEBUG_MODE="OFF"

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
            echo "用法: $0 [选项]"
            echo "选项:"
            echo "  --debug, -d    开启 DEBUG 模式"
            echo "  --release, -r  关闭 DEBUG 模式（默认）"
            echo "  --clean, -c    强制清理 build 目录"
            echo "  --help, -h     显示帮助"
            exit 0
            ;;
        *)
            echo -e "${RED}未知参数: $1${NC}"
            exit 1
            ;;
    esac
done

# ========== 显示当前模式 ==========
echo -e "${GREEN}========================================${NC}"
if [ "$DEBUG_MODE" == "ON" ]; then
    echo -e "${GREEN}  🔧 DEBUG 模式: ${YELLOW}开启${NC}"
else
    echo -e "${GREEN}  🚀 DEBUG 模式: ${YELLOW}关闭${NC}"
fi
echo -e "${GREEN}========================================${NC}"

# ========== 清理构建目录 ==========
if [ "${CLEAN_BUILD:-OFF}" == "ON" ] || [ ! -d "build" ]; then
    echo -e "\n${GREEN}清理旧构建目录...${NC}"
    rm -rf build
fi

# ========== 构建 ==========
echo -e "\n${GREEN}[1/2] 生成构建文件...${NC}"
cmake -S . -B build -DENABLE_DEBUG=$DEBUG_MODE

echo -e "\n${GREEN}[2/2] 开始编译...${NC}"
cmake --build build

echo -e "\n${GREEN}========================================${NC}"
echo -e "${GREEN} ✅ 构建完成！${NC}"
echo -e "${GREEN}========================================${NC}"