cmake_minimum_required(VERSION 3.22)
project(WienerNoder31)

set(CMAKE_CXX_STANDARD 23)

add_executable(WienerNoder31 main.cpp src/City.cpp src/City.hpp src/Pathfinder.cpp src/Pathfinder.hpp src/globals.h)
