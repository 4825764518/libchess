#pragma once

#include "bitboard.h"
#include "square.h"

namespace chess {
Bitboard pawn_move_board(int side, Square square);
Bitboard pawn_double_move_board(int side, Square square);
Bitboard pawn_attack_board(int side, Square square);
Bitboard pawn_attack_board(int side, Bitboard squares);
Bitboard knight_attack_board(Square square);
Bitboard knight_attack_board(Bitboard squares);
Bitboard bishop_attack_board(Bitboard occupied, Square square);
Bitboard bishop_attack_board(Bitboard occupied, Bitboard squares);
Bitboard rook_attack_board(Bitboard occupied, Square square);
Bitboard rook_attack_board(Bitboard occupied, Bitboard squares);
Bitboard queen_attack_board(Bitboard occupied, Square square);
Bitboard queen_attack_board(Bitboard occupied, Bitboard squares);
Bitboard king_attack_board(Square square);
Bitboard king_attack_board(Bitboard squares);
} // namespace chess