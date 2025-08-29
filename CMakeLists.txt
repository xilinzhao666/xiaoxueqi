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

# Find MySQL (optional, using Qt SQL drivers instead)
find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
    pkg_check_modules(MYSQL QUIET mysqlclient)
endif()

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/qt/include)

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

# Qt UI files
set(QT_UI_FILES
    qt/ui/MainWindow.ui
    qt/ui/LoginDialog.ui
    qt/ui/UserRegistrationDialog.ui
    qt/ui/DoctorManagementWidget.ui
    qt/ui/PatientManagementWidget.ui
    qt/ui/AppointmentWidget.ui
    qt/ui/CaseManagementWidget.ui
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