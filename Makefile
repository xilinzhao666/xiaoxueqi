# Makefile for Hospital Management System

# Compiler settings
CXX = /usr/bin/g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic -O2 -I$(INCDIR)
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
LIBS = $(MYSQL_LIBS) $(OPENSSL_LIBS) -lpthread

# Additional include paths
INCLUDE_PATHS = -I$(INCDIR) -I/home/ada/桌面/share/include -I/usr/include/mysql -I/usr/local/include/mysql -I/usr/include/mysql++ -I/usr/include/openssl

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
	@$(CXX) $(CXXFLAGS) $(MYSQL_CFLAGS) $(INCLUDE_PATHS) -c $< -o $@

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
	sudo apt-get install -y pkg-config
	@echo "Dependencies installed!"

# Create database
create-db:
	@echo "Creating database..."
	mysql -u root -p -e "CREATE DATABASE IF NOT EXISTS hospital_db CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;"
	@echo "Database created!"

# Check dependencies
check-deps:
	@echo "Checking dependencies..."
	@which g++ || (echo "g++ not found. Run 'make install-deps'" && exit 1)
	@which mysql_config || (echo "mysql_config not found. Run 'make install-deps'" && exit 1)
	@pkg-config --exists openssl || (echo "OpenSSL not found. Run 'make install-deps'" && exit 1)
	@echo "All dependencies found!"

# Run program
run: $(TARGET)
	@echo "Running $(TARGET)..."
	@./$(TARGET)

# Help
help:
	@echo "Available targets:"
	@echo "  all          - Build the project (default)"
	@echo "  debug        - Build with debug flags"
	@echo "  clean        - Remove build files"
	@echo "  install-deps - Install required dependencies (Ubuntu/Debian)"
	@echo "  check-deps   - Check if dependencies are installed"
	@echo "  create-db    - Create MySQL database"
	@echo "  run          - Build and run the program"
	@echo "  help         - Show this help message"

# Declare phony targets
.PHONY: all debug clean install-deps check-deps create-db run help directories
)
)
)
)
)