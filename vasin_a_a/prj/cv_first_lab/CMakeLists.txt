cmake_minimum_required(VERSION 3.0)

project(cv_first_lab)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(cv_first_lab cv_first_lab.cpp)
target_link_libraries(cv_first_lab ${OpenCV_LIBS})