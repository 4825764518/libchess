#include <libchess/bitboard_iterator.h>
#include <libchess/move_generator.h>
#include <libchess/piece.h>
#include <libchess/square.h>

namespace chess {

void MoveGenerator::generate_pawn_moves(const Board &board, MoveList *moves) {
  const auto &pawn_board = board.pawns();
  const auto side = board.turn();
  const auto &occupied = board.occupied();
  const auto &enemy_occupied = board.occupied(!side);

  BitboardIterator pawn_iter(pawn_board);
  while (pawn_iter.has_data()) {
    auto from = pawn_iter.next();

    auto move_board = pawn_move_board(side, from) & ~occupied;
    if (move_board.data()) {
      move_board |= pawn_double_move_board(side, from) & ~occupied;
    }

    auto capture_board = pawn_attack_board(side, from) & enemy_occupied;

    Bitboard ep_board = 0;
    auto ep_square = board.ep_square();
    if (ep_square != null_square) {
      auto side_offset = side == 0 ? -1 : 1;
      if (ep_square.offset(-1, side_offset) == from ||
          ep_square.offset(1, side_offset) == from) {
        ep_board.set(ep_square);
      }
    }

    constexpr Promotion promotion_types[] = {kPromoteKnight, kPromoteBishop,
                                             kPromoteRook, kPromoteQueen};
    const auto promotion_rank = side == kSideWhite ? 7 : 0;

    // Generate pushes
    BitboardIterator push_iter(move_board);
    while (push_iter.has_data()) {
      auto to = push_iter.next();
      if (to.rank() == promotion_rank) {
        for (auto promote : promotion_types) {
          moves->add_move(chess::Move(from, to, false, false, promote));
        }
      } else {
        moves->add_move(chess::Move(from, to));
      }
    }

    // Generate normal captures
    BitboardIterator capture_iter(capture_board);
    while (capture_iter.has_data()) {
      auto to = capture_iter.next();
      if (to.rank() == promotion_rank) {
        for (auto promote : promotion_types) {
          moves->add_move(chess::Move(from, to, true, false, promote));
        }
      } else {
        moves->add_move(chess::Move(from, to, true));
      }
    }

    // Generate en passant captures
    BitboardIterator ep_iter(ep_board);
    while (ep_iter.has_data()) {
      auto to = ep_iter.next();
      moves->add_move(chess::Move(from, to, true, true));
    }
  }
}

void MoveGenerator::generate_knight_moves(const Board &board, MoveList *moves) {
  const auto &knight_board = board.knights();
  const auto &enemy_occupied = board.occupied(!board.turn());
  BitboardIterator knight_iter(knight_board);
  while (knight_iter.has_data()) {
    auto from = knight_iter.next();

    auto dest_squares =
        knight_attack_board(from) & ~board.occupied(board.turn());
    BitboardIterator dest_iter(dest_squares);
    while (dest_iter.has_data()) {
      auto to = dest_iter.next();
      bool capture = enemy_occupied.occupied(to);
      moves->add_move(chess::Move(from, to, capture));
    }
  }
}

void generate_ray_moves(const Board &board, const Square from,
                        const Bitboard attack_table, MoveList *moves) {

  const auto &enemy_occupied = board.occupied(!board.turn());

  // Filter out captures on our own pieces.
  const auto attacked = attack_table & ~board.occupied(board.turn());

  BitboardIterator attacked_iter(attacked);
  while (attacked_iter.has_data()) {
    auto to = attacked_iter.next();
    const auto capture = enemy_occupied.occupied(to);
    moves->add_move(chess::Move(from, to, capture));
  }
}

void MoveGenerator::generate_bishop_moves(const Board &board, MoveList *moves) {
  const auto &bishops = board.bishops();
  const auto &occupied = board.occupied();

  BitboardIterator bishop_iter(bishops);
  while (bishop_iter.has_data()) {
    auto from = bishop_iter.next();
    const auto attacked = bishop_attack_board(occupied, from);
    generate_ray_moves(board, from, attacked, moves);
  }
}

void MoveGenerator::generate_rook_moves(const Board &board, MoveList *moves) {
  const auto &rooks = board.rooks();
  const auto &occupied = board.occupied();

  BitboardIterator rook_iter(rooks);
  while (rook_iter.has_data()) {
    auto from = rook_iter.next();
    const auto attacked = rook_attack_board(occupied, from);
    generate_ray_moves(board, from, attacked, moves);
  }
}

void MoveGenerator::generate_queen_moves(const Board &board, MoveList *moves) {
  const auto &queens = board.queens();
  const auto &occupied = board.occupied();

  BitboardIterator queen_iter(queens);
  while (queen_iter.has_data()) {
    auto from = queen_iter.next();
    const auto attacked = queen_attack_board(occupied, from);
    generate_ray_moves(board, from, attacked, moves);
  }
}

void MoveGenerator::generate_king_moves(const Board &board, MoveList *moves) {
  const auto &kings = board.kings();
  const auto &occupied = board.occupied(board.turn());
  const auto &enemy_occupied = board.occupied(!board.turn());

  BitboardIterator king_iter(kings);
  while (king_iter.has_data()) {
    auto from = king_iter.next();

    const auto attacked = king_attack_board(from) & ~occupied;
    BitboardIterator attack_iter(attacked);
    while (attack_iter.has_data()) {
      auto to = attack_iter.next();
      const auto capture = enemy_occupied.occupied(to);
      moves->add_move(chess::Move(from, to, capture));
    }
  }
}

void MoveGenerator::generate_castling_moves(chess::Game &game,
                                            MoveList *moves) {
  const auto &board = game.board();
  const auto side = board.turn();
  const auto &king_board = board.kings();

  // G1, C1, G8, C8
  const Square castle_squares[] = {Square(6, 0), Square(2, 0), Square(6, 7),
                                   Square(2, 7)};
  const Square king_squares[] = {Square(4, 0), Square(4, 7)};

  // Generate every pseudolegal move for the opponent. We need this to find out
  // which squares are attacked.
  game.make_null_move();
  auto opponent_moves = generate_pseudolegal_moves(board);
  game.unmake_move();

  Bitboard attacked = 0;
  for (chess::Move move : opponent_moves.moves()) {
    if (move.null()) {
      break;
    }

    attacked.set(move.to());
  }

  if (side == kSideWhite) {
    const Square white_kingside_castle_squares[] = {Square(5, 0), Square(6, 0)};
    const Square white_queenside_castle_squares[] = {Square(1, 0), Square(2, 0),
                                                     Square(3, 0)};

    auto kingside_castle_safe = board.castling(kCastleWhiteKingSide);
    auto queenside_castle_safe = board.castling(kCastleWhiteQueenSide);

    if (kingside_castle_safe) {
      for (auto square : white_kingside_castle_squares) {
        if (board.square_occupied(square) || attacked.occupied(square)) {
          kingside_castle_safe = false;
          break;
        }
      }

      if (!board.square_occupied(Square(7, 0))) {
        kingside_castle_safe = false;
      }
    }

    if (queenside_castle_safe) {
      for (auto square : white_queenside_castle_squares) {
        if (board.square_occupied(square) || attacked.occupied(square)) {
          queenside_castle_safe = false;
          break;
        }
      }

      if (!board.square_occupied(Square(0, 0))) {
        queenside_castle_safe = false;
      }
    }

    if (kingside_castle_safe) {
      moves->add_move(chess::Move(king_squares[side],
                                  castle_squares[kCastleWhiteKingSide]));
    }

    if (queenside_castle_safe) {
      moves->add_move(chess::Move(king_squares[side],
                                  castle_squares[kCastleWhiteQueenSide]));
    }
  } else {
    const Square black_kingside_castle_squares[] = {Square(5, 7), Square(6, 7)};
    const Square black_queenside_castle_squares[] = {Square(1, 7), Square(2, 7),
                                                     Square(3, 7)};
    auto kingside_castle_safe = board.castling(kCastleBlackKingSide);
    auto queenside_castle_safe = board.castling(kCastleBlackQueenSide);

    if (kingside_castle_safe) {
      for (auto square : black_kingside_castle_squares) {
        if (board.square_occupied(square) || attacked.occupied(square)) {
          kingside_castle_safe = false;
          break;
        }
      }

      if (!board.square_occupied(Square(7, 7))) {
        kingside_castle_safe = false;
      }
    }

    if (queenside_castle_safe) {
      for (auto square : black_queenside_castle_squares) {
        if (board.square_occupied(square) || attacked.occupied(square)) {
          queenside_castle_safe = false;
          break;
        }
      }

      if (!board.square_occupied(Square(0, 7))) {
        queenside_castle_safe = false;
      }
    }

    if (kingside_castle_safe) {
      moves->add_move(chess::Move(king_squares[side],
                                  castle_squares[kCastleBlackKingSide]));
    }

    if (queenside_castle_safe) {
      moves->add_move(chess::Move(king_squares[side],
                                  castle_squares[kCastleBlackQueenSide]));
    }
  }
}

MoveList MoveGenerator::generate_pseudolegal_moves(const Board &board) {
  MoveList moves;
  generate_pawn_moves(board, &moves);
  generate_knight_moves(board, &moves);
  generate_bishop_moves(board, &moves);
  generate_rook_moves(board, &moves);
  generate_queen_moves(board, &moves);
  generate_king_moves(board, &moves);
  return moves;
}

MoveList MoveGenerator::generate_legal_moves(chess::Game &game) {
  MoveList legal_moves;
  const auto &board = game.board();
  const auto in_check = board.check(board.turn());
  const auto side = board.turn();

  const auto pseudolegal_moves = generate_pseudolegal_moves(board);
  const auto &moves_array = pseudolegal_moves.moves();
  for (auto i = 0; i < pseudolegal_moves.size(); i++) {
    const auto move = moves_array[i];

    game.make_move(move);
    auto check = board.check(side);
    game.unmake_move();

    if (check) {
      continue;
    }

    legal_moves.add_move(move);
  }

  // Make sure we don't generate castle moves when in check.
  if (!in_check) {
    generate_castling_moves(game, &legal_moves);
  }

  return legal_moves;
}
} // namespace chess