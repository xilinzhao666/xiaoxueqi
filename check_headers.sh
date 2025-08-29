#!/bin/bash

# 检查头文件和依赖的脚本

echo "=== 检查C++编译环境 ==="

# 1. 检查项目头文件目录
echo "1. 检查项目头文件目录..."
if [ -d "/home/ada/桌面/share/include" ]; then
    echo "✅ 项目头文件目录存在: /home/ada/桌面/share/include"
    echo "   包含的头文件:"
    ls -la /home/ada/桌面/share/include/*.h 2>/dev/null || echo "   没有找到.h文件"
else
    echo "❌ 项目头文件目录不存在: /home/ada/桌面/share/include"
fi

# 2. 检查编译工具
echo ""
echo "2. 检查编译工具..."
if command -v g++ &> /dev/null; then
    echo "✅ g++编译器: $(g++ --version | head -1)"
else
    echo "❌ g++编译器未安装"
    echo "   安装命令: sudo apt-get install build-essential"
fi

if command -v cmake &> /dev/null; then
    echo "✅ CMake: $(cmake --version | head -1)"
else
    echo "❌ CMake未安装"
    echo "   安装命令: sudo apt-get install cmake"
fi

# 3. 检查MySQL开发库
echo ""
echo "3. 检查MySQL开发库..."
if command -v mysql_config &> /dev/null; then
    echo "✅ MySQL开发库: $(mysql_config --version)"
    echo "   MySQL头文件路径: $(mysql_config --include)"
    echo "   MySQL库路径: $(mysql_config --libs)"
else
    echo "❌ MySQL开发库未安装"
    echo "   安装命令: sudo apt-get install libmysqlclient-dev"
fi

# 4. 检查MySQL头文件目录
echo ""
echo "4. 检查MySQL头文件目录..."
MYSQL_DIRS=("/usr/include/mysql" "/usr/local/include/mysql" "/usr/include/mysql++")
for dir in "${MYSQL_DIRS[@]}"; do
    if [ -d "$dir" ]; then
        echo "✅ MySQL头文件目录存在: $dir"
        if [ -f "$dir/mysql.h" ]; then
            echo "   ✅ 找到mysql.h"
        else
            echo "   ⚠️  未找到mysql.h"
        fi
    else
        echo "❌ MySQL头文件目录不存在: $dir"
    fi
done

# 5. 检查OpenSSL
echo ""
echo "5. 检查OpenSSL..."
if pkg-config --exists openssl; then
    echo "✅ OpenSSL: $(pkg-config --modversion openssl)"
    echo "   OpenSSL头文件路径: $(pkg-config --cflags openssl)"
else
    echo "❌ OpenSSL开发库未安装"
    echo "   安装命令: sudo apt-get install libssl-dev"
fi

# 6. 检查OpenSSL头文件
if [ -d "/usr/include/openssl" ]; then
    echo "✅ OpenSSL头文件目录存在: /usr/include/openssl"
else
    echo "❌ OpenSSL头文件目录不存在: /usr/include/openssl"
fi

# 7. 检查C++标准库
echo ""
echo "6. 检查C++标准库..."
CPP_DIRS=("/usr/include/c++/11" "/usr/include/c++/9" "/usr/include/c++/10")
for dir in "${CPP_DIRS[@]}"; do
    if [ -d "$dir" ]; then
        echo "✅ C++标准库目录存在: $dir"
        break
    fi
done

echo ""
echo "=== 检查完成 ==="
echo ""
echo "如果发现问题，请运行以下命令安装缺失的依赖:"
echo "sudo apt-get update"
echo "sudo apt-get install build-essential cmake libmysqlclient-dev libssl-dev pkg-config"