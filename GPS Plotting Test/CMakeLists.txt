cmake_minimum_required(VERSION 3.16)
project(GPS_Plotting_Test LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Positioning Location)

# Define the executable and its C++ sources
qt_add_executable(appGPS_Plotting_Test
        main.cpp
        gpsmanager.cpp
        gpsmanager.h
)

# Define the QML module
qt_add_qml_module(appGPS_Plotting_Test
        URI GPS_Plotting_Test
        VERSION 1.0
        QML_FILES main.qml
)

target_link_libraries(appGPS_Plotting_Test PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Qml
        Qt6::Positioning
        Qt6::Location
)