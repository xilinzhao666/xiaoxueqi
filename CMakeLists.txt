cmake_minimum_required(VERSION 3.10)
project(HospitalManagementSystem)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++17")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0 -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2 -DNDEBUG")

# Find required packages
find_package(PkgConfig REQUIRED)
pkg_check_modules(MYSQL REQUIRED mysqlclient)
find_package(OpenSSL REQUIRED)

# Include directories
include_directories(include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${MYSQL_INCLUDE_DIRS})
include_directories(${OPENSSL_INCLUDE_DIR})

# Add compiler-specific include directories
if(CMAKE_COMPILER_IS_GNUCXX)
    include_directories(/usr/include/mysql)
    include_directories(/usr/local/include/mysql)
endif()

# Source files
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
    src/main.cpp
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Set target properties
set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# Link libraries
target_link_libraries(${PROJECT_NAME} 
    ${MYSQL_LIBRARIES}
    ${OPENSSL_LIBRARIES}
    pthread
)

# Compile options
target_compile_options(${PROJECT_NAME} PRIVATE ${MYSQL_CFLAGS_OTHER})

# Compiler warnings
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Debug mode options
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(${PROJECT_NAME} PRIVATE -g -O0)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -O2)
endif()