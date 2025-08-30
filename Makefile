# Makefile for Hospital Management System

# Compiler settings
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic -O2
DEBUGFLAGS = -g -O0 -DDEBUG

# Directory settings
SRCDIR = src
INCDIR = include
OBJDIR = obj
BINDIR = bin

# Library settings
MYSQL_CFLAGS = $(shell mysql_config --cflags)
MYSQL_LIBS = $(shell mysql_config --libs)
OPENSSL_LIBS = -lssl -lcrypto

# 检查nlohmann/json库的位置
JSON_INCLUDE = $(shell pkg-config --cflags nlohmann_json 2>/dev/null || echo "-I/usr/include/nlohmann")
CXXFLAGS += $(JSON_INCLUDE)

LIBS = $(MYSQL_LIBS) $(OPENSSL_LIBS) -lpthread

# Source files and object files
SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
TARGET = $(BINDIR)/HospitalSystem

# Default target
all: directories $(TARGET)

# Create directories
directories:
	@mkdir -p $(OBJDIR) $(BINDIR)

# Link object files
$(TARGET): $(OBJECTS)
	@echo "Linking $@..."
	@$(CXX) $(OBJECTS) -o $@ $(LIBS)
	@echo "Build complete!"

# Compile source files
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@echo "Compiling $<..."
	@$(CXX) $(CXXFLAGS) $(MYSQL_CFLAGS) -I$(INCDIR) -c $< -o $@

# Debug version
debug: CXXFLAGS += $(DEBUGFLAGS)
debug: directories $(TARGET)

# Clean
clean:
	@echo "Cleaning..."
	@rm -rf $(OBJDIR) $(BINDIR)
	@echo "Clean complete!"

# Install dependencies (Ubuntu/Debian)
install-deps:
	@echo "Installing dependencies..."
	sudo apt-get update
	sudo apt-get install -y build-essential cmake
	sudo apt-get install -y libmysqlclient-dev mysql-server
	sudo apt-get install -y libssl-dev
	sudo apt-get install -y nlohmann-json3-dev
	@echo "Dependencies installed!"

# Create database
create-db:
	@echo "Creating database..."
	mysql -u root -p -e "CREATE DATABASE IF NOT EXISTS hospital_db CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;"
	@echo "Database created!"

# Run program
run: $(TARGET)
	@echo "Running $(TARGET)..."
	@./$(TARGET)

# Run with API server functionality
run-api: $(TARGET)
	@echo "Running $(TARGET) with API functionality..."
	@./$(TARGET)

# Help
help:
	@echo "Available targets:"
	@echo "  all          - Build the project (default)"
	@echo "  debug        - Build with debug flags"
	@echo "  clean        - Remove build files"
	@echo "  install-deps - Install required dependencies (Ubuntu/Debian)"
	@echo "  create-db    - Create MySQL database"
	@echo "  run          - Build and run the program"
	@echo "  run-api      - Build and run API server"
	@echo "  help         - Show this help message"

# Declare phony targets
.PHONY: all debug clean install-deps create-db run run-api help directories
