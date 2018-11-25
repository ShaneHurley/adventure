cmake_minimum_required(VERSION 3.10)
project(adventure)

set(CMAKE_CXX_STANDARD 17)

add_executable(adventure src/main.cpp src/item.cpp src/item.h src/room.cpp src/room.h src/game.cpp src/game.h)