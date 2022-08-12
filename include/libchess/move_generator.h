#pragma once

#include <array>

#include "board.h"
#include "game.h"
#include "move.h"

namespace chess {
class MoveList {
public:
  MoveList() : moves_{}, num_moves_(0){};
  MoveList(const std::array<chess::Move, 256> &moves, const int num_moves)
      : moves_(moves), num_moves_(num_moves){};
  MoveList(const MoveList &list) : MoveList(list.moves(), list.size()){};
  void add_move(const chess::Move move) {
    // Unsafe as hell. Very likely to never go above the limit of 256 without a
    // serious logic error or hand made position.
    moves_[num_moves_] = move;
    num_moves_++;
  }
  const std::array<chess::Move, 256> &moves() const { return moves_; }
  chess::Move move(int index) const { return moves_[index]; }
  int size() const { return num_moves_; }

private:
  // Very likely above or near the limit of possible moves on one board.
  std::array<chess::Move, 256> moves_;
  int num_moves_;
};

class MoveGenerator {
public:
  MoveList generate_pseudolegal_moves(const Board &board);
  MoveList generate_legal_moves(chess::Game &game);

private:
  void generate_pawn_moves(const Board &board, MoveList *moves);
  void generate_knight_moves(const Board &board, MoveList *moves);
  void generate_bishop_moves(const Board &board, MoveList *moves);
  void generate_rook_moves(const Board &board, MoveList *moves);
  void generate_queen_moves(const Board &board, MoveList *moves);
  void generate_king_moves(const Board &board, MoveList *moves);
  void generate_castling_moves(Game &game, MoveList *moves);
};
} // namespace chess