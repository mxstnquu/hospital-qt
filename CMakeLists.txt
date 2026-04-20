cmake_minimum_required(VERSION 3.16)
project(hospital-qt LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Sql)

qt_standard_project_setup()

qt_add_executable(hospital-qt
    src/main.cpp
    src/database.cpp
    src/RegistrationWindow.cpp
    src/PatientWindow.cpp
    src/DoctorWindow.cpp
    src/RegistrarWindow.cpp

    include/database.hpp
    include/RegistrationWindow.hpp
    include/PatientWindow.hpp
    include/DoctorWindow.hpp
    include/RegistrarWindow.hpp
)

target_include_directories(hospital-qt
    PRIVATE include
)

target_link_libraries(hospital-qt
    PRIVATE
        Qt6::Core
        Qt6::Widgets
        Qt6::Sql
)