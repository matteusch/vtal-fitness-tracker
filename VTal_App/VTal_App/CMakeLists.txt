cmake_minimum_required(VERSION 3.16)

project(VTal_App VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick Core Gui Qml Bluetooth)

qt_standard_project_setup(REQUIRES 6.10)

qt_add_executable(appVTal_App
    main.cpp
)

qt_add_qml_module(appVTal_App
    URI VTal_App
    QML_FILES
        Main.qml
        SOURCES vtal_backend.h
        QML_FILES WelcomeScreen.qml
        QML_FILES DashboardScreen.qml
        QML_FILES SettingsScreen.qml
        QML_FILES TrainingChoiceScreen.qml
        QML_FILES HistoryScreen.qml
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appVTal_App PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appVTal_App
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appVTal_App PRIVATE
    Qt6::Quick
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Bluetooth
)

install(TARGETS appVTal_App
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
