#pragma once

#include <stdint.h>

namespace chess {
class Square {
public:
  Square() : index_(0) {}
  Square(uint8_t index) : index_(index) {}
  Square(uint8_t file, uint8_t rank) : index_(index(file, rank)) {}
  Square offset(uint8_t file, uint8_t rank) const {
    return Square(index_ + index(file, rank));
  }
  uint8_t index() const { return index_; }
  uint8_t file() const { return index_ % 8; }
  uint8_t rank() const { return (index_ - file()) / 8; }
  bool operator==(const Square other) const { return index_ == other.index(); }
  bool operator!=(const Square other) const { return index_ != other.index(); }
  bool operator<(const Square other) const { return index_ < other.index(); }
  bool operator<=(const Square other) const { return index_ <= other.index(); }
  bool operator>(const Square other) const { return index_ > other.index(); }
  bool operator>=(const Square other) const { return index_ >= other.index(); }
  Square &operator=(uint8_t rhs) {
    index_ = rhs;
    return *this;
  }
  Square &operator++(int) {
    index_ += 1;
    return *this;
  }

private:
  static inline uint8_t index(uint8_t file, uint8_t rank) {
    return (rank * 8) + file;
  }

  uint8_t index_;
};

const Square null_square(0xff);
} // namespace chess