cmake_minimum_required(VERSION 3.18)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS "-O3 -march=native -fopt-info-vec-all")

project(sift_cpp LANGUAGES CXX)

add_subdirectory(src)
add_subdirectory(examples)
