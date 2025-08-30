#!/bin/bash

# 运行单个API测试用例的脚本

if [ $# -ne 1 ]; then
    echo "用法: $0 <测试文件路径>"
    echo "示例: $0 test/public/public_schedule_list_no_filters.json"
    exit 1
fi

test_file=$1
test_name=$(basename "$test_file" .json)
result_file="test/results/${test_name}_result.json"

# 检查测试文件是否存在
if [ ! -f "$test_file" ]; then
    echo "错误: 测试文件不存在: $test_file"
    exit 1
fi

# 检查可执行文件是否存在
if [ ! -f "./build/bin/JsonAPI" ]; then
    echo "错误: JsonAPI可执行文件不存在，请先编译项目"
    echo "运行: make jsonapi"
    exit 1
fi

# 创建结果目录
mkdir -p test/results

echo "=== 运行单个API测试 ==="
echo "测试文件: $test_file"
echo "结果文件: $result_file"
echo ""

echo "测试输入:"
cat "$test_file"
echo ""
echo "---"

# 运行测试
echo "正在运行测试..."
if ./build/bin/JsonAPI --input "$test_file" --output "$result_file"; then
    echo ""
    echo "测试输出:"
    cat "$result_file"
    echo ""
    echo "---"
    echo "测试完成，结果已保存到: $result_file"
else
    echo "测试执行失败"
    exit 1
fi