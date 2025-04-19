cmake_minimum_required(VERSION 3.20)

set(CMAKE_PREFIX_PATH "D:/Qt/6.8.1/mingw_64")

project(nice VERSION 1.0)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

add_subdirectory(FluentUI)
add_subdirectory(src)
