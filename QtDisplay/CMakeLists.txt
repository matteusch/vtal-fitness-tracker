cmake_minimum_required(VERSION 3.16)
project(FitnessTracker VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

qt_add_executable(FitnessTracker WIN32
        main.cpp
        MockSensor.cpp
        DashboardWidgets.cpp
        FitnessTrackerWindow.cpp
)

target_link_libraries(FitnessTracker PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets)