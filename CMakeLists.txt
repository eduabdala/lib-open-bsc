cmake_minimum_required(VERSION 3.15)
project(LIB-OPEN-BSC LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(src) 
add_subdirectory(include) 
add_subdirectory(examples) 
