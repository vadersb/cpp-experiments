cmake_minimum_required(VERSION 3.16)
project(cpp_experiments)

# C++ version
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# debug macro
if (CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-DDEBUG=1)
endif()

# dependencies
include(external_deps.cmake) #external deps

# shared stuff
add_subdirectory(shared_stuff)

# tests
add_subdirectory(tests)

# experiments folder
add_subdirectory(experiments)