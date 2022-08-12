add_executable(chess-board-initialization board_initialization_test.cc)
target_link_libraries(chess-board-initialization libchess)
add_test(NAME chess-board-initialization-test COMMAND chess-board-initialization)

add_executable(chess-move-generator move_generator_test.cc)
target_link_libraries(chess-move-generator libchess)
add_test(NAME chess-move-generator-test COMMAND chess-move-generator)

add_executable(chess-game game_test.cc)
target_link_libraries(chess-game libchess)
add_test(NAME chess-game-test COMMAND chess-game)

