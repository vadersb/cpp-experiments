cmake_minimum_required(VERSION 3.16)
project(cpp_experiments)

# C++ version
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# dependencies
include(external_deps.cmake) #external deps




# shared stuff
add_subdirectory(shared_stuff)

# tests TODO
add_subdirectory(tests)

# experiments folder
add_subdirectory(experiments)