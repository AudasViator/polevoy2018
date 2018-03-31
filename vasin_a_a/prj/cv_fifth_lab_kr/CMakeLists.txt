cmake_minimum_required(VERSION 3.0)

project(cv_fifth_lab_kr)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(cv_fifth_lab_kr cv_fifth_lab_kr.cpp)
target_link_libraries(cv_fifth_lab_kr ${OpenCV_LIBS})