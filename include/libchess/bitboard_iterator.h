#pragma once

#include "bitboard.h"
#include "square.h"

namespace chess {
class BitboardIterator {
public:
  BitboardIterator(const Bitboard board) : board_(board){};
  Square next() {
    auto index = Square(board_.find_first());
    board_.unset(index);
    return index;
  }
  bool has_data() const { return board_.data() != 0; }

private:
  Bitboard board_;
};
} // namespace chess