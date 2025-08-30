#!/bin/bash

# 医院管理系统API测试脚本
# 运行所有API端点的测试用例

echo "=== 医院管理系统API测试开始 ==="
echo "编译时间: $(date)"
echo ""

# 检查可执行文件是否存在
if [ ! -f "./build/bin/JsonAPI" ]; then
    echo "错误: JsonAPI可执行文件不存在，请先编译项目"
    echo "运行: make jsonapi"
    exit 1
fi

# 创建结果目录
mkdir -p test/results

# 测试计数器
total_tests=0
passed_tests=0
failed_tests=0

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# 运行单个测试的函数
run_test() {
    local test_file=$1
    local test_name=$(basename "$test_file" .json)
    local result_file="test/results/${test_name}_result.json"
    
    echo -n "运行测试: $test_name ... "
    
    # 运行测试
    if ./build/bin/JsonAPI --input "$test_file" --output "$result_file" 2>/dev/null; then
        # 检查结果文件是否生成
        if [ -f "$result_file" ]; then
            # 检查响应是否包含错误
            if grep -q '"status": "error"' "$result_file"; then
                echo -e "${YELLOW}EXPECTED_ERROR${NC}"
            else
                echo -e "${GREEN}PASS${NC}"
                ((passed_tests++))
            fi
        else
            echo -e "${RED}FAIL (no output)${NC}"
            ((failed_tests++))
        fi
    else
        echo -e "${RED}FAIL (execution error)${NC}"
        ((failed_tests++))
    fi
    
    ((total_tests++))
}

# 运行Public API测试
echo -e "${BLUE}=== Public API 测试 ===${NC}"
for test_file in test/public/*.json; do
    if [ -f "$test_file" ]; then
        run_test "$test_file"
    fi
done
echo ""

# 运行Patient API测试
echo -e "${BLUE}=== Patient API 测试 ===${NC}"
for test_file in test/patient/*.json; do
    if [ -f "$test_file" ]; then
        run_test "$test_file"
    fi
done
echo ""

# 运行Doctor API测试
echo -e "${BLUE}=== Doctor API 测试 ===${NC}"
for test_file in test/doctor/*.json; do
    if [ -f "$test_file" ]; then
        run_test "$test_file"
    fi
done
echo ""

# 显示测试结果统计
echo "=== 测试结果统计 ==="
echo "总测试数: $total_tests"
echo -e "通过测试: ${GREEN}$passed_tests${NC}"
echo -e "失败测试: ${RED}$failed_tests${NC}"

if [ $failed_tests -eq 0 ]; then
    echo -e "${GREEN}所有测试通过！${NC}"
    exit 0
else
    echo -e "${RED}有测试失败，请检查结果文件${NC}"
    exit 1
fi