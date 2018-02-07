cmake_minimum_required(VERSION 3.9)
project(Snake)

set(CMAKE_CXX_STANDARD 11)

add_executable(Snake main.cpp scape.cpp keys.cpp keys.h)