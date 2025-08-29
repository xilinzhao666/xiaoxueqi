#!/bin/bash

# 安装C++编译所需的所有依赖

echo "=== 安装C++编译依赖 ==="

# 更新包列表
echo "1. 更新包列表..."
sudo apt-get update

# 安装基础编译工具
echo "2. 安装基础编译工具..."
sudo apt-get install -y build-essential

# 安装CMake
echo "3. 安装CMake..."
sudo apt-get install -y cmake

# 安装MySQL开发库
echo "4. 安装MySQL开发库..."
sudo apt-get install -y libmysqlclient-dev

# 安装MySQL服务器（如果需要）
echo "5. 安装MySQL服务器..."
sudo apt-get install -y mysql-server

# 安装OpenSSL开发库
echo "6. 安装OpenSSL开发库..."
sudo apt-get install -y libssl-dev

# 安装pkg-config
echo "7. 安装pkg-config..."
sudo apt-get install -y pkg-config

# 安装其他可能需要的库
echo "8. 安装其他依赖..."
sudo apt-get install -y libc6-dev

echo ""
echo "=== 安装完成 ==="
echo ""
echo "现在运行检查脚本验证安装:"
echo "./check_headers.sh"