#!/bin/bash

# 自动退出脚本，如果任何一条命令执行失败
set -euo pipefail

# 颜色输出
RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m' # No Color

echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN} 准备清理旧构建目录...${NC}"
echo -e "${GREEN}========================================${NC}"

# 🔥 每次编译前强制删除旧的 build 文件夹
# rm -rf build

echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN} 开始执行 CMake 构建...${NC}"
echo -e "${GREEN}========================================${NC}"

# 1. 生成构建系统
echo -e "\n${GREEN}[1/2] 生成构建文件...${NC}"
cmake -S . -B build

# 2. 开始编译
echo -e "\n${GREEN}[2/2] 开始编译...${NC}"
cmake --build build

echo -e "\n${GREEN}========================================${NC}"
echo -e "${GREEN} 构建完成！${NC}"
echo -e "${GREEN} 可执行文件在 build 目录下${NC}"
echo -e "${GREEN}========================================${NC}"