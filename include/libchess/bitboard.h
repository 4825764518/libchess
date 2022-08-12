#pragma once

#include <stdint.h>

#include "platform.h"
#include "square.h"

namespace chess {
class Bitboard {
public:
  Bitboard() : data_(0){};
  Bitboard(uint64_t data) : data_(data){};
  Bitboard(const Bitboard &board) : data_(board.data()){};
  Bitboard &operator=(const uint64_t data) {
    data_ = data;
    return *this;
  };
  uint64_t data() const { return data_; };
  uint64_t occupied(Square square) const {
    return (data_ & mask(square.index()));
  };
  int find_first() const {
    assert(data_ > 0);

    return platform::find_first_set(data_);
  }
  int find_last() const {
    if (!data_)
      return 0;
    return platform::find_last_set(data_);
  }
  int count() const { return platform::popcount(data_); }
  void set(Square square) { data_ |= mask(square.index()); };
  void set(Bitboard board) { data_ |= board.data(); }
  void unset(Square square) { data_ &= ~mask(square.index()); };
  void unset(Bitboard board) { data_ &= ~board.data(); };
  Bitboard operator|(const Bitboard &other) const {
    return Bitboard(data_ | other.data());
  }
  Bitboard operator&(const Bitboard &other) const {
    return Bitboard(data_ & other.data());
  }
  Bitboard operator^(const Bitboard &other) const {
    return Bitboard(data_ ^ other.data());
  }
  Bitboard operator~() const { return Bitboard(~data_); }
  Bitboard &operator|=(const Bitboard &other) {
    set(other);
    return *this;
  }
  Bitboard &operator&=(const Bitboard &other) {
    data_ = data_ & other.data();
    return *this;
  }
  Bitboard &operator^=(const Bitboard &other) {
    data_ = data_ ^ other.data();
    return *this;
  }
  bool operator==(const Bitboard &other) const { return data_ == other.data(); }
  bool operator!=(const Bitboard &other) const { return !(*this == other); }

private:
  uint64_t mask(int index) const { return 1ull << index; }

  uint64_t data_;
};
} // namespace chess