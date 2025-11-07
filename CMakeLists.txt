cmake_minimum_required(VERSION 4.0)
project(NoveAguas LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

# This enables AUTOMOC, AUTOUIC, and AUTORCC
qt_standard_project_setup()

add_executable(NoveAguas
        src/main.cpp
        src/AppWindow.h
        src/AppWindow.cpp
        src/SettingsDialog.cpp
        src/SettingsDialog.h
        src/ProgressWidget.cpp
        src/ProgressWidget.h
)

qt_add_resources(NoveAguas "resources"
    PREFIX "/"
    FILES
        assets/glass_empty.png
        assets/glass_full.png
)

target_link_libraries(NoveAguas PRIVATE Qt6::Widgets)

set_target_properties(NoveAguas PROPERTIES
        #WIN32_EXECUTABLE TRUE
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/build"
)