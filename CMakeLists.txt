cmake_minimum_required(VERSION 3.14)

project(DocxReader LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core REQUIRED)


include_directories("${CMAKE_SOURCE_DIR}/include")

FILE(GLOB SRC_H "${CMAKE_SOURCE_DIR}/include/*.h")
FILE(GLOB SRC_CPP "${CMAKE_SOURCE_DIR}/src/*.cpp")
SET(SRC ${SRC_H} ${SRC_CPP})


project(DocxReader)

add_library(DocxReader ${SRC})
target_link_libraries(DocxReader Qt${QT_VERSION_MAJOR}::Core zip)
