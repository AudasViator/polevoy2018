cmake_minimum_required(VERSION 3.0)

project(cv_third_lab)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(cv_third_lab cv_third_lab.cpp)
target_link_libraries(cv_third_lab ${OpenCV_LIBS})