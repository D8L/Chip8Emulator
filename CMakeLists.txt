cmake_minimum_required(VERSION 3.25)
project(Chip8)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake_modules)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})

add_executable(Chip8 src/main.cpp src/chip8.cpp src/chip8.h)

target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARY})