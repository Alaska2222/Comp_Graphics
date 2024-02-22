cmake_minimum_required(VERSION 3.27)
project(GC)

set(CMAKE_CXX_STANDARD 17)
include_directories(C:/MinGW/include)
add_executable(GC main.cpp
)

target_link_libraries(GC -lopengl32 -lfreeglut)
