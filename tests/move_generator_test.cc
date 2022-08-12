#include <bitset>
#include <iostream>

#include <libchess/game.h>
#include <libchess/move_generator.h>

int count_moves(const chess::Board board) {
  chess::MoveGenerator move_generator;
  auto moves = move_generator.generate_pseudolegal_moves(board);

  return moves.size();
}

int count_legal_moves(const chess::Board board) {
  chess::Game game(board);
  chess::MoveGenerator move_generator;
  auto moves = move_generator.generate_legal_moves(game);

  return moves.size();
}

int count_castling_moves(const chess::Board board) {
  chess::Game game(board);
  chess::MoveGenerator move_generator;
  auto moves = move_generator.generate_legal_moves(game);
  int castling_moves = 0;
  for (auto move : moves.moves()) {
    if (move.null()) {
      break;
    }

    if (move.castling(board)) {
      castling_moves++;
    }
  }

  return castling_moves;
}

bool test_pawn_moves() {
  bool failed = false;

  // A single row of pawns in their starting rank - should always be 8 single
  // moves and 8 double moves.
  {
    auto board = chess::Board::from_fen("8/8/8/8/8/8/PPPPPPPP/8 w - - 0 1");
    if (count_moves(board) != 16) {
      failed = true;
    }
  }

  // Same as above but with black pieces.
  {
    auto board = chess::Board::from_fen("8/pppppppp/8/8/8/8/8/8 b - - 0 1");
    if (count_moves(board) != 16) {
      failed = true;
    }
  }

  // Single pawn with two moves and two captures.
  {
    auto board = chess::Board::from_fen("8/8/8/8/8/q1q5/1P6/8 w - - 0 1");
    if (count_moves(board) != 4) {
      failed = true;
    }
  }

  // Single pawn with no moves and two captures.
  {
    auto board = chess::Board::from_fen("8/8/8/8/8/qqq5/1P6/8 w - - 0 1");
    if (count_moves(board) != 2) {
      failed = true;
    }
  }

  // En passant capture possible on g6
  {
    auto board = chess::Board::from_fen("8/8/8/5Pp1/8/8/8/8 w - g6 0 1");
    if (count_moves(board) != 2) {
      failed = true;
    }
  }

  // Same as above but for black.
  {
    auto board = chess::Board::from_fen("8/8/8/8/Pp6/8/8/8 b - a3 0 1");
    if (count_moves(board) != 2) {
      failed = true;
    }
  }

  // En passant is possible but no piece is around to capture.
  {
    auto board = chess::Board::from_fen("8/8/8/8/P6p/8/8/8 b - a3 0 1");
    if (count_moves(board) != 1) {
      failed = true;
    }
  }

  return failed;
}

bool test_knight_moves() {
  bool failed = false;

  // Single knight in the corner of the board - should always be two moves.
  {
    auto board = chess::Board::from_fen("8/8/8/8/8/8/8/N7 w - - 0 1");
    if (count_moves(board) != 2) {
      failed = true;
    }
  }

  return failed;
}

bool test_bishop_moves() {
  bool failed = false;

  // Single bishop in the bottom right corner of the board - should always be 7
  // moves.
  {
    auto board = chess::Board::from_fen("8/8/8/8/8/8/8/B7 w - - 0 1");
    if (count_moves(board) != 7) {
      failed = true;
    }
  }

  // Bishop near the center of the board - should always be 13 moves.
  {
    auto board = chess::Board::from_fen("8/8/8/4B3/8/8/8/8 w - - 0 1");
    if (count_moves(board) != 13) {
      failed = true;
    }
  }

  // Surrounded bishop. 4 possible captures.
  {
    auto board = chess::Board::from_fen("8/8/8/2q1q3/3B4/2q1q3/8/8 w - - 0 1");
    if (count_moves(board) != 4) {
      failed = true;
    }
  }

  return failed;
}

bool test_rook_moves() {
  bool failed = false;

  // Single rook in the corner of the board - should always be 14 moves.
  {
    auto board = chess::Board::from_fen("8/8/8/8/8/8/8/R7 w - - 0 1");
    if (count_moves(board) != 14) {
      failed = true;
    }
  }

  // Two rooks in the corner - should always be 20 moves
  {
    auto board = chess::Board::from_fen("8/8/8/8/8/8/8/RR6 w - - 0 1");
    if (count_moves(board) != 20) {
      failed = true;
    }
  }

  return failed;
}

bool test_queen_moves() {
  bool failed = false;

  // Queen in the center of an empty board. Should always be 27 moves.
  {
    auto board = chess::Board::from_fen("8/8/8/8/3Q4/8/8/8 w - - 0 1");
    if (count_moves(board) != 27) {
      failed = true;
    }
  }

  // Surrounded queen. 8 possible captures.
  {
    auto board =
        chess::Board::from_fen("8/8/8/2qqq3/2qQq3/2qqq3/8/8 w - - 0 1");
    if (count_moves(board) != 8) {
      failed = true;
    }
  }

  return failed;
}

bool test_king_moves() {
  bool failed = false;

  // King in the center of an empty board. Should always be 8 moves.
  {
    auto board = chess::Board::from_fen("8/8/8/4K3/8/8/8/8 w - - 0 1");
    if (count_moves(board) != 8) {
      failed = true;
    }
  }

  {
    auto board = chess::Board::from_fen("K7/2k5/8/8/8/8/8/8 w - - 0 1");
    if (count_legal_moves(board) != 1) {
      failed = true;
    }
  }

  {
    auto board = chess::Board::from_fen("8/b1k1b3/8/2K5/8/8/8/8 w - - 0 1");
    if (count_legal_moves(board) != 3) {
      failed = true;
    }
  }

  {
    auto board = chess::Board::from_fen("8/2k5/3q4/2K5/2Q5/8/8/8 w - - 0 1");
    if (count_legal_moves(board) != 1) {
      failed = true;
    }
  }

  {
      auto board = chess::Board::from_fen("8/8/8/2k5/3Pp3/8/8/4K3 b - d3 0 1");
      auto moves = count_legal_moves(board);
      std::cout << "moves: " << moves << std::endl;
  }

  return failed;
}

bool test_castling_moves() {
  bool failed = false;

  // White can castle kingside.
  {
    auto board = chess::Board::from_fen("8/8/8/8/8/8/8/4K2R w K - 0 1");
    if (count_castling_moves(board) != 1) {
      failed = true;
    }
  }

  // White has kingside castling rights but castling is blocked by a piece.
  {
    auto board = chess::Board::from_fen("8/8/8/8/8/8/8/4KN1R w K - 0 1");
    if (count_castling_moves(board) != 0) {
      failed = true;
    }
  }

  // Black can castle on either side, but it's queenside castle is blocked by an
  // attacking piece
  {
    auto board = chess::Board::from_fen("r3k2r/8/8/8/8/8/8/1Q6 b KQkq - 0 1");
    if (count_castling_moves(board) != 1) {
      failed = true;
    }
  }

  return failed;
}

bool test_pin_moves() {
  bool failed = false;

  // Bishop pinned by queen. The king can make 5 legal moves.
  {
    auto board = chess::Board::from_fen("8/8/8/8/2qBK3/8/8/8 w - - 0 1");
    if (count_legal_moves(board) != 5) {
      failed = true;
    }
  }

  {
    auto board = chess::Board::from_fen(
        "r1bqkbnr/ppp2ppp/2np4/1B2p3/4P3/5N2/PPPP1PPP/RN1QKB1R b KQkq - 0 1");
    if (count_legal_moves(board) != 27) {
      failed = true;
    }
  }

  return failed;
}

bool test_promotion_moves() {
  bool failed = false;

  {
    auto board = chess::Board::from_fen("8/2P5/8/8/8/8/8/8 w - - 0 1");
    if (count_moves(board) != 4) {
      failed = true;
    }
  }

  // Promotion either by pushing or capturing
  {
    auto board = chess::Board::from_fen("3q4/2P5/8/8/8/8/8/8 w - - 0 1");
    if (count_moves(board) != 8) {
      failed = true;
    }
  }

  // Promotion blocked
  {
    auto board = chess::Board::from_fen("2k5/2P5/8/8/8/8/8/8 w - - 0 1");
    if (count_moves(board) != 0) {
      failed = true;
    }
  }

  return failed;
}

int main() {
  bool failed = false;

  if (test_pawn_moves()) {
    std::cerr << "Pawn move generation test failed" << std::endl;
    failed = true;
  }

  if (test_knight_moves()) {
    std::cerr << "Knight move generation test failed" << std::endl;
    failed = true;
  }

  if (test_bishop_moves()) {
    std::cerr << "Bishop move generation test failed" << std::endl;
    failed = true;
  }

  if (test_rook_moves()) {
    std::cerr << "Rook move generation test failed" << std::endl;
    failed = true;
  }

  if (test_queen_moves()) {
    std::cerr << "Queen move generation test failed" << std::endl;
    failed = true;
  }

  if (test_king_moves()) {
    std::cerr << "King move generation test failed" << std::endl;
    failed = true;
  }

  if (test_castling_moves()) {
    std::cerr << "Castling move generation test failed" << std::endl;
    failed = true;
  }

  if (test_pin_moves()) {
    std::cerr << "Pinned piece move generation test failed" << std::endl;
    failed = true;
  }

  if (test_promotion_moves()) {
    std::cerr << "Promotion move generation test failed" << std::endl;
    failed = true;
  }

  return failed;
}