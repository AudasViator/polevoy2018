cmake_minimum_required(VERSION 3.0)

project(cv_second_lab)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(cv_second_lab cv_second_lab.cpp)
target_link_libraries(cv_second_lab ${OpenCV_LIBS})