cmake_minimum_required(VERSION 3.10)
enable_testing()

project(libchess VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(libchess src/board.cc src/game.cc src/move_generator.cc src/piece.cc)
target_include_directories(libchess PUBLIC include/)

add_subdirectory(tests)
add_subdirectory(benchmarks)