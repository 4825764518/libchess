#include <libchess/game.h>
#include <libchess/move_generator.h>
#include <stdlib.h>

#include <iostream>
#include <chrono>

int main() {
  constexpr auto num_iterations = 10000000;

  chess::Game game;
  chess::MoveGenerator move_generator;
  auto start_time = std::chrono::system_clock::now();
  for (auto i = 0; i < num_iterations; i++) {
    auto moves = move_generator.generate_legal_moves(game);
    auto size = moves.size();

    auto move_idx = rand() % size;
    game.make_move(moves.move(move_idx));
    if (game.drawn() || game.board().check(chess::kSideBlack) ||
        game.board().check(chess::kSideWhite)) {
      game = chess::Game();
    }
  }
  auto end_time = std::chrono::system_clock::now();
  auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
  std::cout << "Delta: " << delta.count() << "ms average: " << delta.count() / num_iterations << "ms" << std::endl;
  std::flush(std::cout);

  return 0;
}