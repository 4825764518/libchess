#pragma once

#include <vector>

#include "board.h"
#include "move.h"

namespace chess {
class Game {
public:
  Game() : Game(default_fen) {}
  Game(const Board &board) : board_(board) {}
  Game(const std::string &fen) : board_(Board::from_fen(fen)) {}

  void make_move(const Move move);
  void make_null_move();
  void unmake_move();

  const Board &board() const { return board_; }

  bool drawn() const;

private:
  bool is_stalemate() const;
  bool is_fifty_move() const;
  bool is_repetition() const;

  inline static const std::string default_fen =
      "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
  // The current board state, reflecting the last move's effect.
  Board board_;
  // Past board states.
  std::vector<Board> old_boards_;
};
} // namespace chess