#include <assert.h>
#include <libchess/game.h>

#include <iostream>

namespace chess {
void Game::make_move(Move move) {
  assert(move.null() == false);

  auto &old_board = board_;
  old_boards_.push_back(old_board);

  const auto side = old_board.turn();
  const auto from = move.from();
  const auto to = move.to();
  const auto from_piece_type = old_board.piece_type_at(side, move.from());

  auto ep_square = null_square;
  const auto ep_direction = side == kSideWhite ? -1 : 1;
  const auto ep_offset = to.offset(0, ep_direction * 2);
  if (from_piece_type == kPiecePawn && ep_offset == from) {
    ep_square = to.offset(0, ep_direction);
  }

  if (move.castling(old_board)) {
    // Move the king and the rook here.
    auto king_board = old_board.piece_board(side, kPieceKing);
    auto rook_board = old_board.piece_board(side, kPieceRook);

    king_board.unset(from);
    king_board.set(to);

    auto rook_rank = to.rank();
    auto old_rook_file = to.file() == 2 ? 0 : 7;
    auto new_rook_file = to.file() == 2 ? 3 : 5;
    auto old_rook_square = Square(
        old_rook_file, rook_rank); // square_index(old_rook_file, rook_rank);
    auto new_rook_square = Square(
        new_rook_file, rook_rank); // square_index(new_rook_file, rook_rank);
    rook_board.unset(old_rook_square);
    rook_board.set(new_rook_square);

    old_board.set_piece_board(side, kPieceKing, king_board);
    old_board.set_piece_board(side, kPieceRook, rook_board);
  } else {
    auto move_piece_board = old_board.piece_board(side, from_piece_type);
    move_piece_board.unset(from);

    // check for promotion
    auto promote_rank = side == kSideWhite ? 7 : 0;
    if (from_piece_type == kPiecePawn && to.rank() == promote_rank) {
      auto promote_type = move.promotion_piece_type();
      auto promote_board = old_board.piece_board(side, promote_type);
      promote_board.set(to);
      old_board.set_piece_board(side, promote_type, promote_board);
    } else {
      move_piece_board.set(to);
    }

    old_board.set_piece_board(side, from_piece_type, move_piece_board);
  }

  if (move.capture()) {
    Square capture_square = to;
    if (move.en_passant()) {
      const auto ep_capture_direction = side == kSideWhite ? -1 : 1;
      capture_square = to.offset(
          0,
          ep_capture_direction); // square_offset(to, 0, ep_capture_direction);
    }

    auto capture_side = !side;
    const auto capture_piece_type =
        old_board.piece_type_at(capture_side, capture_square);
    auto capture_piece_board =
        old_board.piece_board(capture_side, capture_piece_type);
    capture_piece_board.unset(capture_square);
    old_board.set_piece_board(capture_side, capture_piece_type,
                              capture_piece_board);

    if (capture_piece_type == kPieceRook) {
      if (capture_side == kSideWhite) {
        if (to == Square(0, 0)) {
          old_board.set_castling(kCastleWhiteQueenSide, false);
        } else if (to == Square(7, 0)) {
          old_board.set_castling(kCastleWhiteKingSide, false);
        }
      } else {
        if (to == Square(0, 7)) {
          old_board.set_castling(kCastleBlackQueenSide, false);
        } else if (to == Square(7, 7)) {
          old_board.set_castling(kCastleBlackKingSide, false);
        }
      }
    }
  }

  // Update half move clock
  auto half_move = old_board.half_move();
  if (move.capture() || from_piece_type == kPiecePawn) {
    half_move = 0;
  } else {
    half_move++;
  }

  if (from_piece_type == kPieceKing) {
    // King moved. Clear out castling rights for this side.
    if (side == chess::kSideWhite) {
      old_board.set_castling(kCastleWhiteKingSide, false);
      old_board.set_castling(kCastleWhiteQueenSide, false);
    } else {
      old_board.set_castling(kCastleBlackKingSide, false);
      old_board.set_castling(kCastleBlackQueenSide, false);
    }
  } else if (from_piece_type == kPieceRook) {
    // Rook moved. Clear castling rights if we moved out of it's initial
    // position.
    if (side == kSideWhite) {
      if (from == Square(0, 0)) {
        old_board.set_castling(kCastleWhiteQueenSide, false);
      } else if (from == Square(7, 0)) {
        old_board.set_castling(kCastleWhiteKingSide, false);
      }
    } else {
      if (from == Square(0, 7)) {
        old_board.set_castling(kCastleBlackQueenSide, false);
      } else if (from == Square(7, 7)) {
        old_board.set_castling(kCastleBlackKingSide, false);
      }
    }
  }

  old_board.set_ep_square(ep_square);
  old_board.set_half_move(half_move);
  old_board.set_turn(!side);
  old_board.set_full_move(old_board.full_move() + side);
  old_board.update_occupied();
}

void Game::make_null_move() {
  // auto &old_move = move_;
  auto &old_board = board_;

  // old_moves_.push_back(old_move);
  old_boards_.push_back(old_board);

  const auto side = old_board.turn();
  old_board.set_turn(!side);

  old_board.set_half_move(0);
}

void Game::unmake_move() {
  // move_ = old_moves_.back();
  board_ = old_boards_.back();
  // old_moves_.pop_back();
  old_boards_.pop_back();
}

bool Game::is_stalemate() const {
  auto occupied = board_.occupied();
  if (occupied.count() == 2) {
    // There's only 2 kings left on the board.
    return true;
  }

  return false;
}

bool Game::is_fifty_move() const {
  auto half_move = board_.half_move();
  return half_move >= 50;
}

bool Game::is_repetition() const {
  int count = 0;
  for (auto board : old_boards_) {
    if (board == board_) {
      count++;
    }
  }

  return count >= 2;
}

bool Game::drawn() const {
  return is_stalemate() || is_fifty_move() || is_repetition();
}

} // namespace chess