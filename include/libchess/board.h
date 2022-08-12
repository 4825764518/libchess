#pragma once

#include <stdint.h>

#include <string>
#include <vector>

#include "bitboard.h"
#include "square.h"

namespace chess {
enum Side {
  kSideWhite = 0,
  kSideBlack = 1,
  kNumSides,
};

enum CastlingRights {
  kCastleWhiteKingSide = 0,
  kCastleWhiteQueenSide = 1,
  kCastleBlackKingSide = 2,
  kCastleBlackQueenSide = 3,
  kNumCastle,
};

enum PieceType {
  kPiecePawn = 0,
  kPieceKnight = 1,
  kPieceBishop = 2,
  kPieceRook = 3,
  kPieceQueen = 4,
  kPieceKing = 5,
  kNumPieces,
};

class Board {
public:
  Board(std::vector<Bitboard> pawns, std::vector<Bitboard> knights,
        std::vector<Bitboard> bishops, std::vector<Bitboard> rooks,
        std::vector<Bitboard> queens, std::vector<Bitboard> kings,
        std::vector<bool> castling, int side, Square ep_square, int half_move,
        int full_move);
  static Board from_fen(std::string fen);
  bool operator==(const Board &board) const;
  bool operator!=(const Board &board) const { return !(*this == board); };

  std::string fen() const;

  const Bitboard &pawns(int side) const { return pieces_[side][kPiecePawn]; }
  const Bitboard &knights(int side) const {
    return pieces_[side][kPieceKnight];
  }
  const Bitboard &bishops(int side) const {
    return pieces_[side][kPieceBishop];
  }
  const Bitboard &rooks(int side) const { return pieces_[side][kPieceRook]; }
  const Bitboard &queens(int side) const { return pieces_[side][kPieceQueen]; }
  const Bitboard &kings(int side) const { return pieces_[side][kPieceKing]; }
  const Bitboard &pawns() const { return pawns(side_); }
  const Bitboard &knights() const { return knights(side_); }
  const Bitboard &bishops() const { return bishops(side_); }
  const Bitboard &rooks() const { return rooks(side_); }
  const Bitboard &queens() const { return queens(side_); }
  const Bitboard &kings() const { return kings(side_); }

  const Bitboard &piece_board(int side, int piece_type) const {
    return pieces_[side][piece_type];
  }
  void set_piece_board(int side, int piece_type, Bitboard board) {
    pieces_[side][piece_type] = board;
  }
  int piece_type_at(int side, Square square) const {
    if (!square_occupied(square))
      return -1;

    for (auto i = 0; i < kNumPieces; i++) {
      auto &piece_board = pieces_[side][i];
      if (piece_board.occupied(square))
        return i;
    }

    return -1;
  }
  int piece_type_at(Square square) const {
    auto piece = piece_type_at(kSideWhite, square);
    if (piece == -1) {
      piece = piece_type_at(kSideBlack, square);
    }

    return piece;
  }

  const Bitboard &occupied(int side) const { return occupied_[side]; }
  const Bitboard occupied() const {
    return occupied_[kSideWhite] | occupied_[kSideBlack];
  }
  void update_occupied() {
    for (int side = 0; side < kNumSides; side++) {
      occupied_[side] = 0;

      for (int piece = 0; piece < kNumPieces; piece++) {
        occupied_[side] |= piece_board(side, piece);
      }
    }
  }

  bool castling(int castle_side) const { return castling_[castle_side]; }
  void set_castling(int castle_side, bool value) {
    castling_[castle_side] = value;
  }

  int turn() const { return side_; };
  void set_turn(int side) { side_ = side; }

  Square ep_square() const { return ep_square_; };
  void set_ep_square(Square ep_square) { ep_square_ = ep_square; }

  int half_move() const { return half_move_; };
  void set_half_move(int half_move) { half_move_ = half_move; }

  int full_move() const { return full_move_; };
  void set_full_move(int full_move) { full_move_ = full_move; }

  bool square_occupied(int side, Square square) const {
    return this->occupied_[side].occupied(square);
  };

  bool square_occupied(Square square) const {
    return square_occupied(kSideWhite, square) ||
           square_occupied(kSideBlack, square);
  };

  uint64_t attacks_to_square(int side, Square square) const;
  bool check(int side) const;

private:
  // Bitboards representing each side's pieces.
  Bitboard pieces_[kNumSides][kNumPieces];
  Bitboard occupied_[kNumSides];
  // Castling rights per side and per side of the board (king side and queen
  // side)
  bool castling_[kNumCastle];
  // The side to play. 0 = white, 1 = black.
  int side_;
  int half_move_;
  int full_move_;
  // Square for en passant captures. -1 if no en passant capture is possible
  // this turn.
  Square ep_square_;
};

extern Board null_board;
} // namespace chess