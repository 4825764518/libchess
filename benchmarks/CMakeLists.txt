add_executable(move-generator-benchmark move_generator_benchmark.cc)
target_link_libraries(move-generator-benchmark libchess)

add_executable(perft perft.cc)
target_link_libraries(perft libchess)