#!/bin/bash

# 医院管理系统编译修复脚本
# 解决头文件路径和依赖问题

echo "=== 医院管理系统编译修复脚本 ==="

# 检查是否在正确的目录
if [ ! -f "CMakeLists.txt" ] && [ ! -f "Makefile" ]; then
    echo "错误：请在项目根目录运行此脚本"
    exit 1
fi

# 1. 检查并安装依赖
echo "1. 检查系统依赖..."

# 检查编译工具
if ! command -v g++ &> /dev/null; then
    echo "安装编译工具..."
    sudo apt-get update
    sudo apt-get install -y build-essential
fi

# 检查CMake
if ! command -v cmake &> /dev/null; then
    echo "安装CMake..."
    sudo apt-get install -y cmake
fi

# 检查MySQL开发库
if ! command -v mysql_config &> /dev/null; then
    echo "安装MySQL开发库..."
    sudo apt-get install -y libmysqlclient-dev mysql-server
fi

# 检查OpenSSL
if ! pkg-config --exists openssl; then
    echo "安装OpenSSL开发库..."
    sudo apt-get install -y libssl-dev
fi

# 检查pkg-config
if ! command -v pkg-config &> /dev/null; then
    echo "安装pkg-config..."
    sudo apt-get install -y pkg-config
fi

echo "2. 依赖检查完成！"

# 2. 创建必要的目录
echo "3. 创建构建目录..."
mkdir -p obj bin build

# 3. 检查头文件路径
echo "4. 检查头文件路径..."

# 查找MySQL头文件
MYSQL_INCLUDE_DIRS=$(mysql_config --include 2>/dev/null || echo "-I/usr/include/mysql")
echo "MySQL头文件路径: $MYSQL_INCLUDE_DIRS"

# 查找OpenSSL头文件
if [ -d "/usr/include/openssl" ]; then
    echo "OpenSSL头文件路径: /usr/include/openssl"
else
    echo "警告：未找到OpenSSL头文件"
fi

# 4. 尝试编译
echo "5. 开始编译..."

# 方法1：使用Make
if [ -f "Makefile" ]; then
    echo "使用Make编译..."
    make clean
    if make; then
        echo "✅ Make编译成功！"
        echo "运行程序: ./bin/HospitalSystem"
        exit 0
    else
        echo "❌ Make编译失败，尝试CMake..."
    fi
fi

# 方法2：使用CMake
if [ -f "CMakeLists.txt" ]; then
    echo "使用CMake编译..."
    cd build
    if cmake .. && make; then
        echo "✅ CMake编译成功！"
        echo "运行程序: ./build/HospitalManagementSystem"
        exit 0
    else
        echo "❌ CMake编译也失败"
        cd ..
    fi
fi

# 5. 如果都失败，提供诊断信息
echo "6. 编译失败，提供诊断信息..."

echo "系统信息："
echo "- 操作系统: $(uname -a)"
echo "- GCC版本: $(gcc --version | head -1)"
echo "- G++版本: $(g++ --version | head -1)"
echo "- CMake版本: $(cmake --version | head -1)"
echo "- MySQL配置: $(mysql_config --version 2>/dev/null || echo '未找到')"

echo ""
echo "请检查以下问题："
echo "1. 是否安装了所有依赖？运行: sudo apt-get install build-essential cmake libmysqlclient-dev libssl-dev"
echo "2. MySQL服务是否运行？运行: sudo systemctl start mysql"
echo "3. 头文件路径是否正确？检查 /usr/include/mysql 是否存在"
echo "4. 如果使用VS Code，重启VS Code以刷新IntelliSense"

echo ""
echo "如需帮助，请提供上述系统信息。"