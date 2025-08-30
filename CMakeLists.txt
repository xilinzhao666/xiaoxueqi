cmake_minimum_required(VERSION 3.10)
project(HospitalManagementSystem)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置编译选项
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -O2")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0 -DDEBUG")

# 查找依赖库
find_package(PkgConfig REQUIRED)

# MySQL
pkg_check_modules(MYSQL REQUIRED mysqlclient)

# OpenSSL
find_package(OpenSSL REQUIRED)

# Threads
find_package(Threads REQUIRED)

# 包含目录
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${MYSQL_INCLUDE_DIRS})

# 添加nlohmann/json库（如果系统没有安装，可以作为子模块添加）
find_package(nlohmann_json 3.2.0 QUIET)
if(NOT nlohmann_json_FOUND)
    # 如果没有找到，尝试使用pkg-config
    pkg_check_modules(NLOHMANN_JSON nlohmann_json)
    if(NOT NLOHMANN_JSON_FOUND)
        message(WARNING "nlohmann/json not found. Please install it manually.")
        # 可以添加FetchContent来自动下载
        include(FetchContent)
        FetchContent_Declare(json
            GIT_REPOSITORY https://github.com/nlohmann/json.git
            GIT_TAG v3.11.2)
        FetchContent_MakeAvailable(json)
    endif()
endif()

# 源文件
set(SOURCES
    src/DatabaseConnection.cpp
    src/User.cpp
    src/Doctor.cpp
    src/Patient.cpp
    src/Case.cpp
    src/Appointment.cpp
    src/Hospitalization.cpp
    src/Prescription.cpp
    src/Medication.cpp
    src/HospitalService.cpp
    src/ApiHandler.cpp
)

# 创建库
add_library(HospitalLib ${SOURCES})

# 链接库到HospitalLib
target_link_libraries(HospitalLib 
    ${MYSQL_LIBRARIES}
    OpenSSL::SSL 
    OpenSSL::Crypto
    Threads::Threads
)

if(nlohmann_json_FOUND)
    target_link_libraries(HospitalLib nlohmann_json::nlohmann_json)
else()
    target_link_libraries(HospitalLib nlohmann_json)
endif()

# 主程序可执行文件
add_executable(Terminal src/main.cpp)
target_link_libraries(Terminal HospitalLib)

# JSON API可执行文件
add_executable(JsonAPI src/JsonAPI.cpp)
target_link_libraries(JsonAPI HospitalLib)

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/lib)

set_target_properties(Terminal PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/bin
)

set_target_properties(JsonAPI PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/bin
)

install(TARGETS Terminal JsonAPI
    RUNTIME DESTINATION bin
)

install(TARGETS HospitalLib
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# 测试（可选）
enable_testing()

# 如果有测试文件
if(EXISTS ${CMAKE_SOURCE_DIR}/tests)
    add_subdirectory(tests)
endif()