cmake_minimum_required(VERSION 3.16)
project(QtHospitalSystem VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt6 components
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Sql)

# Enable Qt MOC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/qt/include
    /home/ada/桌面/share/include
)

# Find MySQL
find_package(PkgConfig REQUIRED)
pkg_check_modules(MYSQL REQUIRED mysqlclient)

# Find OpenSSL
find_package(OpenSSL REQUIRED)

# Add include directories
include_directories(${MYSQL_INCLUDE_DIRS})
include_directories(${OPENSSL_INCLUDE_DIR})

# Qt source files
set(QT_SOURCES
    qt/src/main.cpp
    qt/src/MainWindow.cpp
    qt/src/LoginDialog.cpp
    qt/src/UserRegistrationDialog.cpp
    qt/src/DoctorManagementWidget.cpp
    qt/src/PatientManagementWidget.cpp
    qt/src/AppointmentWidget.cpp
    qt/src/CaseManagementWidget.cpp
    qt/src/DatabaseManager.cpp
    qt/src/HospitalBusinessLogic.cpp
)

# Qt header files
set(QT_HEADERS
    qt/include/MainWindow.h
    qt/include/LoginDialog.h
    qt/include/UserRegistrationDialog.h
    qt/include/DoctorManagementWidget.h
    qt/include/PatientManagementWidget.h
    qt/include/AppointmentWidget.h
    qt/include/CaseManagementWidget.h
    qt/include/DatabaseManager.h
    qt/include/HospitalBusinessLogic.h
)

# Create executable
add_executable(QtHospitalSystem ${QT_SOURCES} ${QT_HEADERS})

# Link Qt libraries
target_link_libraries(QtHospitalSystem 
    Qt6::Core 
    Qt6::Widgets 
    Qt6::Sql
)

# Set target properties
set_target_properties(QtHospitalSystem PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

# Install target
install(TARGETS QtHospitalSystem
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)