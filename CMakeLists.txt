cmake_minimum_required(VERSION 3.15)
project(MCEvolution)

set(JSON_BuildTests OFF CACHE INTERNAL "")

set(CMAKE_CXX_STANDARD 17)

find_library(LIB_TO_INCLUDE PyreNet lib)
find_path(LIB_INCLUDES PyreNet.h lib/PyreNet)

set(EIGEN_DIR ${CMAKE_SOURCE_DIR}/lib/eigen-3.3.9)

include_directories(${LIB_INCLUDES})
include_directories(${EIGEN_DIR})

add_executable(MCEvolution
        main.cpp
        Game.cpp
        Game.h
        World.cpp
        World.h
        Player.cpp
        Player.h
        Block.cpp
        Block.h
        RayCaster.cpp
        RayCaster.h
        constants.h
        lib/nlohmann/json.hpp)

target_link_libraries(MCEvolution ${LIB_TO_INCLUDE})

target_compile_options(MCEvolution PUBLIC -O3)