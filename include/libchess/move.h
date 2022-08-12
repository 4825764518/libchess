#pragma once

#include <cstdint>

#include "board.h"
#include "square.h"

namespace chess {
enum Promotion {
  kPromoteQueen = 0,
  kPromoteRook = 1,
  kPromoteBishop = 2,
  kPromoteKnight = 3,
};

#pragma pack(push, 1)
class Move {
public:
  Move(Square from = 0, Square to = 0, bool capture = false, bool ep = false,
       Promotion promotion = kPromoteQueen)
      : from_(from.index()), to_(to.index()), capture_(capture), ep_(ep),
        promotion_(promotion){};

  Square from() const { return from_; };
  Square to() const { return to_; };
  bool capture() const { return capture_; };
  bool en_passant() const { return ep_; }
  uint32_t promotion() const { return promotion_; }
  uint32_t promotion_piece_type() const {
    constexpr int piece_types[] = {kPieceQueen, kPieceRook, kPieceBishop,
                                   kPieceKnight};
    return piece_types[promotion_];
  }
  bool castling(const Board &board) const {
    auto kings = board.kings();
    if (!kings.occupied(from_)) {
      return false;
    }

    auto from_square = from();
    auto to_square = to();

    if (from_square.rank() != to_square.rank()) {
      return false;
    }

    auto distance = abs(from_square.file() - to_square.file());
    return distance > 1;
  }
  bool null() const { return !from_ && !to_; }

  bool operator==(const chess::Move &move) const {
    return from() == move.from() && to() == move.to() &&
           capture_ == move.capture() && ep_ == move.en_passant() &&
           promotion_ == move.promotion();
  }
  bool operator!=(const chess::Move &move) const { return !(*this == move); }

private:
  uint8_t from_ : 6;
  uint8_t to_ : 6;
  bool capture_ : 1;
  // En passant
  bool ep_ : 1;
  uint32_t promotion_ : 2;
};
#pragma pack(pop)
// static_assert(sizeof(Move) == 2, "Bad chess::Move size");
} // namespace chess