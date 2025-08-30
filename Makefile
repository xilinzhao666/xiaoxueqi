# Makefile for Hospital Management System - Dual Executables
# 医院管理系统 - 双可执行文件版本

# 编译器设置
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic -O2
DEBUGFLAGS = -g -O0 -DDEBUG

# 目录设置
SRCDIR = src
INCDIR = include
OBJDIR = obj
BINDIR = bin
LIBDIR = lib

# 库设置
MYSQL_CFLAGS = $(shell mysql_config --cflags 2>/dev/null || echo "-I/usr/include/mysql")
MYSQL_LIBS = $(shell mysql_config --libs 2>/dev/null || echo "-lmysqlclient")
OPENSSL_LIBS = -lssl -lcrypto

# 检查nlohmann/json库的位置
JSON_INCLUDE = $(shell pkg-config --cflags nlohmann_json 2>/dev/null || echo "-I/usr/include")
CXXFLAGS += $(JSON_INCLUDE)

LIBS = $(MYSQL_LIBS) $(OPENSSL_LIBS) -lpthread

# 共享源文件（不包含main函数的文件）
SHARED_SOURCES = $(SRCDIR)/DatabaseConnection.cpp \
                 $(SRCDIR)/User.cpp \
                 $(SRCDIR)/Doctor.cpp \
                 $(SRCDIR)/Patient.cpp \
                 $(SRCDIR)/Case.cpp \
                 $(SRCDIR)/Appointment.cpp \
                 $(SRCDIR)/Hospitalization.cpp \
                 $(SRCDIR)/Prescription.cpp \
                 $(SRCDIR)/Medication.cpp \
                 $(SRCDIR)/HospitalService.cpp \
                 $(SRCDIR)/ApiHandler.cpp

# 共享对象文件
SHARED_OBJECTS = $(SHARED_SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

# 可执行文件目标
TERMINAL_TARGET = $(BINDIR)/Terminal
JSONAPI_TARGET = $(BINDIR)/JsonAPI
SHARED_LIB = $(LIBDIR)/libhospital.a

# 默认目标 - 编译所有可执行文件
all: directories $(TERMINAL_TARGET) $(JSONAPI_TARGET)
	@echo "=== 编译完成 ==="
	@echo "Terminal可执行文件: $(TERMINAL_TARGET)"
	@echo "JsonAPI可执行文件: $(JSONAPI_TARGET)"

# 创建必要的目录
directories:
	@mkdir -p $(OBJDIR) $(BINDIR) $(LIBDIR)

# 编译共享库
$(SHARED_LIB): $(SHARED_OBJECTS)
	@echo "创建共享库 $@..."
	@ar rcs $@ $(SHARED_OBJECTS)
	@echo "共享库创建完成!"

# Terminal可执行文件 - 终端交互模式
$(TERMINAL_TARGET): $(SHARED_LIB) $(OBJDIR)/main.o
	@echo "链接Terminal可执行文件 $@..."
	@$(CXX) $(OBJDIR)/main.o -L$(LIBDIR) -lhospital $(LIBS) -o $@
	@echo "Terminal编译完成!"

# JsonAPI可执行文件 - JSON文件处理模式
$(JSONAPI_TARGET): $(SHARED_LIB) $(OBJDIR)/JsonAPI.o
	@echo "链接JsonAPI可执行文件 $@..."
	@$(CXX) $(OBJDIR)/JsonAPI.o -L$(LIBDIR) -lhospital $(LIBS) -o $@
	@echo "JsonAPI编译完成!"

# 编译共享源文件的对象文件
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@echo "编译共享源文件 $<..."
	@$(CXX) $(CXXFLAGS) $(MYSQL_CFLAGS) -I$(INCDIR) -c $< -o $@

# 编译main.cpp对象文件
$(OBJDIR)/main.o: $(SRCDIR)/main.cpp
	@echo "编译Terminal主程序 $<..."
	@$(CXX) $(CXXFLAGS) $(MYSQL_CFLAGS) -I$(INCDIR) -c $< -o $@

# 编译JsonAPI.cpp对象文件
$(OBJDIR)/JsonAPI.o: $(SRCDIR)/JsonAPI.cpp
	@echo "编译JsonAPI主程序 $<..."
	@$(CXX) $(CXXFLAGS) $(MYSQL_CFLAGS) -I$(INCDIR) -c $< -o $@

# 单独编译目标
terminal: directories $(TERMINAL_TARGET)
	@echo "Terminal可执行文件编译完成: $(TERMINAL_TARGET)"

jsonapi: directories $(JSONAPI_TARGET)
	@echo "JsonAPI可执行文件编译完成: $(JSONAPI_TARGET)"

# 调试版本
debug: CXXFLAGS += $(DEBUGFLAGS)
debug: directories $(TERMINAL_TARGET) $(JSONAPI_TARGET)

# 清理编译文件
clean:
	@echo "清理编译文件..."
	@rm -rf $(OBJDIR) $(BINDIR) $(LIBDIR)
	@rm -f test_results/*.json 2>/dev/null || true
	@echo "清理完成!"

# 安装依赖库 (Ubuntu/Debian)
install-deps:
	@echo "安装依赖库..."
	sudo apt-get update
	sudo apt-get install -y build-essential cmake
	sudo apt-get install -y libmysqlclient-dev mysql-server
	sudo apt-get install -y libssl-dev
	sudo apt-get install -y nlohmann-json3-dev
	@echo "依赖库安装完成!"

# 创建数据库
create-db:
	@echo "创建数据库..."
	mysql -u root -p -e "CREATE DATABASE IF NOT EXISTS hospital_db CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;"
	@echo "数据库创建完成!"

# 运行Terminal程序
run-terminal: $(TERMINAL_TARGET)
	@echo "启动Terminal程序..."
	@./$(TERMINAL_TARGET)

# 测试JsonAPI功能
test-jsonapi: $(JSONAPI_TARGET)
	@echo "测试JsonAPI功能..."
	@mkdir -p test_results
	@echo '{"api": "public.schedule.list", "data": {}}' > test_results/test_input.json
	@./$(JSONAPI_TARGET) --input test_results/test_input.json --output test_results/test_output.json
	@echo "测试请求:"
	@cat test_results/test_input.json
	@echo ""
	@echo "测试响应:"
	@cat test_results/test_output.json
	@echo ""

# 运行完整的JsonAPI测试套件
test-jsonapi-full: $(JSONAPI_TARGET)
	@echo "运行完整JsonAPI测试..."
	@chmod +x scripts/test_jsonapi.sh
	@./scripts/test_jsonapi.sh

# 显示帮助信息
help:
	@echo "可用的编译目标:"
	@echo "  all              - 编译所有可执行文件 (默认)"
	@echo "  terminal         - 仅编译Terminal可执行文件"
	@echo "  jsonapi          - 仅编译JsonAPI可执行文件"
	@echo "  debug            - 编译调试版本"
	@echo "  clean            - 清理编译文件"
	@echo ""
	@echo "依赖和环境:"
	@echo "  install-deps     - 安装编译依赖库 (Ubuntu/Debian)"
	@echo "  create-db        - 创建MySQL数据库"
	@echo ""
	@echo "运行和测试:"
	@echo "  run-terminal     - 编译并运行Terminal程序"
	@echo "  test-jsonapi     - 编译并测试JsonAPI基本功能"
	@echo "  test-jsonapi-full- 运行完整JsonAPI测试套件"
	@echo "  help             - 显示此帮助信息"
	@echo ""
	@echo "使用示例:"
	@echo "  make all                    # 编译所有程序"
	@echo "  make terminal && make run-terminal  # 编译并运行Terminal"
	@echo "  make jsonapi && make test-jsonapi   # 编译并测试JsonAPI"

# 声明伪目标
.PHONY: all terminal jsonapi debug clean install-deps create-db run-terminal test-jsonapi test-jsonapi-full help directories

# 依赖关系
$(TERMINAL_TARGET): $(SHARED_LIB)
$(JSONAPI_TARGET): $(SHARED_LIB)
$(SHARED_LIB): $(SHARED_OBJECTS)