cmake_minimum_required(VERSION 3.0)

project(cv_hello_word)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(cv_hello_word cv_hello_word.cpp)
target_link_libraries(cv_hello_word ${OpenCV_LIBS})