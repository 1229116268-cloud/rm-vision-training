cmake_minimum_required(VERSION 3.10)
project(VisionTraining)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 告诉 CMake 去 Homebrew 的默认安装路径找库
set(CMAKE_PREFIX_PATH "/opt/homebrew")

find_package(OpenCV 4 REQUIRED)
add_executable(task1_image src/main.cpp)

target_include_directories(task1_image PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(task1_image PRIVATE ${OpenCV_LIBS})