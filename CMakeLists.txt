cmake_minimum_required(VERSION 3.10)

set(CXXFLAGS "-O0 -Wall")
set(CMAKE_BUILD_TYPE Debug)

project(VideoMotionDetector)

find_package(OpenCV REQUIRED)

add_executable(VideoMotionDetector main.cpp)

target_link_libraries(VideoMotionDetector PRIVATE ${OpenCV_LIBS})
