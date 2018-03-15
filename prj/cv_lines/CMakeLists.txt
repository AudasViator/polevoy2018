cmake_minimum_required(VERSION 3.0)

project(cv_lines)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(cv_lines cv_lines.cpp)
target_link_libraries(cv_lines ${OpenCV_LIBS})