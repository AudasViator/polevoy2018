cmake_minimum_required(VERSION 3.0)

project(cv_fourth_lab)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(cv_fourth_lab cv_fourth_lab.cpp)
target_link_libraries(cv_fourth_lab ${OpenCV_LIBS})