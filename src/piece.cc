#include <array>
#include <tuple>

#include <libchess/bitboard_iterator.h>
#include <libchess/board.h>
#include <libchess/piece.h>

namespace chess {

const std::array<std::array<Bitboard, 64>, kNumSides>
compute_pawn_move_tables() {
  std::array<std::array<Bitboard, 64>, kNumSides> move_tables;
  for (auto side = 0; side < kNumSides; side++) {
    const auto direction = side == 0 ? 1 : -1;
    std::array<Bitboard, 64> moves;
    for (Square square = 0; square < 64; square++) {
      auto file = square.file();
      auto rank = square.rank();
      Bitboard board = 0;

      auto rank_offset = rank + direction;
      if (rank_offset < 0 || rank_offset > 7)
        continue;

      board.set(Square(file, rank_offset));
      moves[square.index()] = board;
    }

    move_tables[side] = moves;
  }

  return move_tables;
}

const std::array<std::array<Bitboard, 64>, kNumSides>
compute_pawn_double_move_tables() {
  std::array<std::array<Bitboard, 64>, kNumSides> move_tables;
  for (auto side = 0; side < kNumSides; side++) {
    const auto direction = side == 0 ? 2 : -2;
    const auto double_move_rank = side == chess::kSideWhite ? 1 : 6;
    std::array<Bitboard, 64> moves;
    for (Square square = 0; square < 64; square++) {
      Bitboard board = 0;

      if (square.rank() == double_move_rank) {
        board.set(Square(square.file(), double_move_rank + direction));
      }

      moves[square.index()] = board;
    }

    move_tables[side] = moves;
  }

  return move_tables;
}

const std::array<std::array<Bitboard, 64>, kNumSides>
compute_pawn_capture_tables() {
  constexpr int offsets[] = {-1, 1};

  std::array<std::array<Bitboard, 64>, kNumSides> capture_tables;
  for (auto side = 0; side < kNumSides; side++) {
    auto direction = side == 0 ? 1 : -1;
    std::array<Bitboard, 64> moves;
    for (Square square = 0; square < 64; square++) {
      auto file = square.file();
      auto rank = square.rank();
      Bitboard board = 0;

      for (auto offset : offsets) {
        auto file_offset = file + offset;
        auto rank_offset = rank + direction;
        if (file_offset < 0 || file_offset > 7)
          continue;
        if (rank_offset < 0 || rank_offset > 7)
          continue;
        board.set(Square(file_offset, rank_offset));
      }

      moves[square.index()] = board;
    }

    capture_tables[side] = moves;
  }

  return capture_tables;
}

const std::array<Bitboard, 64> compute_knight_table() {
  constexpr std::tuple<int, int> offsets[] = {
      {-2, 1}, {-1, 2}, {1, 2}, {2, 1}, {2, -1}, {1, -2}, {-1, -2}, {-2, -1}};
  std::array<Bitboard, 64> moves = {0};
  for (Square square = 0; square < 64; square++) {
    auto file = square.file();
    auto rank = square.rank();
    Bitboard board = 0;

    for (auto offset : offsets) {
      auto file_offset = file + std::get<0>(offset);
      auto rank_offset = rank + std::get<1>(offset);
      if (file_offset < 0 || file_offset > 7)
        continue;
      if (rank_offset < 0 || rank_offset > 7)
        continue;
      board.set(Square(file_offset, rank_offset));
    }

    moves[square.index()] = board;
  }

  return moves;
}

const std::array<Bitboard, 64> compute_king_table() {
  constexpr std::tuple<int, int> offsets[] = {
      {-1, -1}, {-1, 0}, {-1, 1}, {0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1}};
  std::array<Bitboard, 64> moves = {0};
  for (Square square = 0; square < 64; square++) {
    auto file = square.file();
    auto rank = square.rank();
    Bitboard board = 0;

    for (auto offset : offsets) {
      auto file_offset = file + std::get<0>(offset);
      auto rank_offset = rank + std::get<1>(offset);
      if (file_offset < 0 || file_offset > 7)
        continue;
      if (rank_offset < 0 || rank_offset > 7)
        continue;
      board.set(Square(file_offset, rank_offset));
    }

    moves[square.index()] = board;
  }

  return moves;
}

template <int file_offset, int rank_offset>
const std::array<Bitboard, 64> compute_ray_table() {
  std::array<Bitboard, 64> moves = {0};
  for (Square square = 0; square < 64; square++) {
    Bitboard board = 0;
    auto file = square.file() + file_offset;
    auto rank = square.rank() + rank_offset;

    while (true) {
      if (file < 0 || file > 7)
        break;
      if (rank < 0 || rank > 7)
        break;

      board.set(Square(file, rank));

      file += file_offset;
      rank += rank_offset;
    }

    moves[square.index()] = board;
  }

  return moves;
}

// Attack look up tables. Saves a few cycles not having to compute these at
// runtime.
const std::array<std::array<Bitboard, 64>, kNumSides> pawn_move_tables =
    compute_pawn_move_tables();
const std::array<std::array<Bitboard, 64>, kNumSides> pawn_double_move_tables =
    compute_pawn_double_move_tables();
const std::array<std::array<Bitboard, 64>, kNumSides> pawn_capture_tables =
    compute_pawn_capture_tables();
const std::array<Bitboard, 64> knight_attack_table = compute_knight_table();
const std::array<Bitboard, 64> king_attack_table = compute_king_table();

// Lookup tables for bishop/rook/queen directional attacks
const std::array<Bitboard, 64> ray_n_table = compute_ray_table<0, 1>();
const std::array<Bitboard, 64> ray_e_table = compute_ray_table<1, 0>();
const std::array<Bitboard, 64> ray_s_table = compute_ray_table<0, -1>();
const std::array<Bitboard, 64> ray_w_table = compute_ray_table<-1, 0>();

const std::array<Bitboard, 64> ray_nw_table = compute_ray_table<-1, 1>();
const std::array<Bitboard, 64> ray_ne_table = compute_ray_table<1, 1>();
const std::array<Bitboard, 64> ray_se_table = compute_ray_table<1, -1>();
const std::array<Bitboard, 64> ray_sw_table = compute_ray_table<-1, -1>();

template <bool generate_diagonal_rays, bool generate_orthogonal_rays>
inline Bitboard ray_attack_table(const Bitboard occupied, const Square from) {
  Bitboard attacked = 0;
  Bitboard blockers = 0;
  Square blocking_square;

  if (generate_diagonal_rays) {
    auto nw_attacked = ray_nw_table[from.index()];
    blockers = (nw_attacked & occupied) | 0x8000000000000000ull;
    blocking_square = blockers.find_first();
    nw_attacked ^= ray_nw_table[blocking_square.index()];

    auto ne_attacked = ray_ne_table[from.index()];
    blockers = (ne_attacked & occupied) | 0x8000000000000000ull;
    blocking_square = blockers.find_first();
    ne_attacked ^= ray_ne_table[blocking_square.index()];

    auto se_attacked = ray_se_table[from.index()];
    blockers = (se_attacked & occupied) | 1ull;
    blocking_square = blockers.find_last();
    se_attacked ^= ray_se_table[blocking_square.index()];

    auto sw_attacked = ray_sw_table[from.index()];
    blockers = (sw_attacked & occupied) | 1ull;
    blocking_square = blockers.find_last();
    sw_attacked ^= ray_sw_table[blocking_square.index()];

    attacked |= nw_attacked | ne_attacked | se_attacked | sw_attacked;
  }

  if (generate_orthogonal_rays) {
    auto n_attacked = ray_n_table[from.index()];
    blockers = (n_attacked & occupied) | 0x8000000000000000ull;
    blocking_square = blockers.find_first();
    n_attacked ^= ray_n_table[blocking_square.index()];

    auto e_attacked = ray_e_table[from.index()];
    blockers = (e_attacked & occupied) | 0x8000000000000000ull;
    blocking_square = blockers.find_first();
    e_attacked ^= ray_e_table[blocking_square.index()];

    auto s_attacked = ray_s_table[from.index()];
    blockers = (s_attacked & occupied) | 1ull;
    blocking_square = blockers.find_last();
    s_attacked ^= ray_s_table[blocking_square.index()];

    auto w_attacked = ray_w_table[from.index()];
    blockers = (w_attacked & occupied) | 1ull;
    blocking_square = blockers.find_last();
    w_attacked ^= ray_w_table[blocking_square.index()];

    attacked |= n_attacked | e_attacked | s_attacked | w_attacked;
  }

  return attacked;
}

Bitboard pawn_move_board(int side, Square square) {
  return pawn_move_tables[side][square.index()];
}

Bitboard pawn_double_move_board(int side, Square square) {
  return pawn_double_move_tables[side][square.index()];
}

Bitboard pawn_attack_board(int side, Square square) {
  return pawn_capture_tables[side][square.index()];
}

Bitboard pawn_attack_board(int side, Bitboard squares) {
  Bitboard attacked = 0;

  BitboardIterator square_iter(squares);
  while (square_iter.has_data()) {
    auto square = square_iter.next();
    attacked.set(pawn_attack_board(side, square));
  }

  return attacked;
}

Bitboard knight_attack_board(Square square) {
  return knight_attack_table[square.index()];
}

Bitboard knight_attack_board(Bitboard squares) {
  Bitboard attacked = 0;

  BitboardIterator square_iter(squares);
  while (square_iter.has_data()) {
    auto square = square_iter.next();
    attacked.set(knight_attack_board(square));
  }

  return attacked;
}

Bitboard bishop_attack_board(Bitboard occupied, Square square) {
  return ray_attack_table<true, false>(occupied, square);
}

Bitboard bishop_attack_board(Bitboard occupied, Bitboard squares) {
  Bitboard attacked = 0;

  BitboardIterator square_iter(squares);
  while (square_iter.has_data()) {
    auto square = square_iter.next();
    attacked.set(bishop_attack_board(occupied, square));
  }

  return attacked;
}

Bitboard rook_attack_board(Bitboard occupied, Square square) {
  return ray_attack_table<false, true>(occupied, square);
}

Bitboard rook_attack_board(Bitboard occupied, Bitboard squares) {
  Bitboard attacked = 0;

  BitboardIterator square_iter(squares);
  while (square_iter.has_data()) {
    auto square = square_iter.next();
    attacked.set(rook_attack_board(occupied, square));
  }

  return attacked;
}

Bitboard queen_attack_board(Bitboard occupied, Square square) {
  return ray_attack_table<true, true>(occupied, square);
}

Bitboard queen_attack_board(Bitboard occupied, Bitboard squares) {
  Bitboard attacked = 0;

  BitboardIterator square_iter(squares);
  while (square_iter.has_data()) {
    auto square = square_iter.next();
    attacked.set(queen_attack_board(occupied, square));
  }

  return attacked;
}

Bitboard king_attack_board(Square square) {
  return king_attack_table[square.index()];
}

Bitboard king_attack_board(Bitboard squares) {
  Bitboard attacked = 0;

  BitboardIterator square_iter(squares);
  while (square_iter.has_data()) {
    auto square = square_iter.next();
    attacked.set(king_attack_board(square));
  }

  return attacked;
}
} // namespace chess