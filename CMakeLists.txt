cmake_minimum_required(VERSION 3.18)
project(FileSyncer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

IF (MSVC) # implies windows only
    IF (CMAKE_BUILD_TYPE MATCHES Release)
        set(CMAKE_PREFIX_PATH "E:/Qt/6.5.0/msvc2019_64-static-release/lib/cmake")

        add_compile_options($<$<CONFIG:Release>:/MT>)
    ELSEIF (CMAKE_BUILD_TYPE MATCHES Debug)
        set(CMAKE_PREFIX_PATH "E:/Qt/6.5.0/msvc2019_64/lib/cmake")
    ENDIF ()
ELSEIF (APPLE)
ELSEIF (UNIX AND NOT APPLE)
ENDIF ()

# Include the Botan Library
add_library(Botan STATIC)
target_sources(
        Botan
        PUBLIC
        E:/Botan/Botan-3.0.0/botan_all.h
        PRIVATE
        E:/Botan/Botan-3.0.0/botan_all.cpp
)
target_include_directories(
        Botan
        PUBLIC
        E:/Botan/Botan-3.0.0
)

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        Network
        REQUIRED)

add_executable(FileSyncer main.cpp MainWindow.cpp MainWindow.h MainWindow.ui NetworkManager.cpp NetworkManager.h KMessageWidget.cpp KMessageWidget.h)

target_link_libraries(FileSyncer
        Qt::Core
        Qt::Gui
        Qt::Widgets
        Qt::Network
        Botan
        )

IF (CMAKE_BUILD_TYPE MATCHES Release)
    # Let CMake generate the executable for windows without a console
    # This has to be placed last for some reason
    set_target_properties(FileSyncer PROPERTIES WIN32_EXECUTABLE TRUE)
ENDIF ()