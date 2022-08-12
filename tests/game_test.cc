#include <iostream>

#include <libchess/game.h>
#include <libchess/square.h>

constexpr char piece_symbols[] = {'p', 'n', 'b', 'r', 'q', 'k'};

void print_board(chess::Board &board) {
  for (auto y = 7; y >= 0; y--) {
    for (auto x = 0; x < 8; x++) {
      auto square = chess::Square(x, y);
      if (board.square_occupied(square)) {
        auto side = board.turn();
        auto piece_type = board.piece_type_at(side, square);
        if (piece_type == -1) {
          side = !side;
          piece_type = board.piece_type_at(side, square);
        }

        auto symbol = piece_symbols[piece_type];
        if (side == chess::kSideWhite) {
          symbol = toupper(symbol);
        }

        std::cerr << symbol;
      } else {
        std::cerr << ".";
      }
    }

    std::cerr << std::endl;
  }
}

bool test_fen() {
  bool failed = false;

  // Test to make sure that the original FEN string is reproduced with no state
  // change
  {
    auto board = chess::Board::from_fen(
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    auto fen = board.fen();
    if (fen.compare(
            "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1") != 0) {
      failed = true;
    }
  }

  // Testing for proper game state after 1. a4
  {
    chess::Game game;
    game.make_move(chess::Move(8, 24));

    // Note that this FEN string can vary depending on implementation. Lichess
    // does not record the uncapturable en passant square.
    auto fen = game.board().fen();
    if (fen.compare(
            "rnbqkbnr/pppppppp/8/8/P7/8/1PPPPPPP/RNBQKBNR b KQkq a3 0 1") !=
        0) {
      std::cerr << "Bad fen: " << fen << std::endl;
      failed = true;
    }
  }

  return failed;
}

bool test_castling_rights() {
  bool failed = false;

  // Test loss of castling rights after a capture.
  {
    chess::Game game("4k3/8/8/8/8/8/6p1/4K2R b KQkq - 0 1");
    game.make_move(chess::Move(chess::Square(6, 1), chess::Square(7, 0), true));

    if (game.board().castling(chess::kCastleWhiteKingSide)) {
      failed = true;
    }
  }

  // Test loss of castling rights after moving a rook
  {
    chess::Game game("4k3/8/8/8/8/8/6p1/4K2R w KQkq - 0 1");
    game.make_move(chess::Move(chess::Square(7, 0), chess::Square(6, 0)));

    if (game.board().castling(chess::kCastleWhiteKingSide)) {
      failed = true;
    }
  }

  // Test loss of castling rights after moving a king
  {
    chess::Game game(
        "rnbqk2r/pp1p2Np/8/4p3/1bP5/2nBP3/PPQ2PPP/R1B1K2R b KQkq - 0 10");
    game.make_move(chess::Move(chess::Square(4, 7), chess::Square(4, 6)));

    auto board = game.board();
    if (board.castling(chess::kCastleBlackKingSide) ||
        board.castling(chess::kCastleBlackQueenSide)) {
      failed = true;
    }
  }

  return failed;
}

bool test_castling() {
  bool failed = false;

  {
    chess::Game game("r3k2r/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR b KQkq - 0 1");
    game.make_move(chess::Move(chess::Square(4, 7), chess::Square(2, 7)));

    auto board = game.board();
    if (board.piece_type_at(chess::kSideBlack, chess::Square(2, 7)) !=
            chess::kPieceKing ||
        board.piece_type_at(chess::kSideBlack, chess::Square(3, 7)) !=
            chess::kPieceRook) {
      failed = true;
    }
  }

  {
    chess::Game game("r3k2r/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR b KQkq - 0 1");
    game.make_move(chess::Move(chess::Square(4, 7), chess::Square(6, 7)));

    auto board = game.board();
    if (board.piece_type_at(chess::kSideBlack, chess::Square(6, 7)) !=
            chess::kPieceKing ||
        board.piece_type_at(chess::kSideBlack, chess::Square(5, 7)) !=
            chess::kPieceRook) {
      failed = true;
    }
  }

  {
    chess::Game game("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1");
    game.make_move(chess::Move(chess::Square(4, 0), chess::Square(6, 0)));

    auto board = game.board();
    if (board.piece_type_at(chess::kSideWhite, chess::Square(6, 0)) !=
            chess::kPieceKing ||
        board.piece_type_at(chess::kSideWhite, chess::Square(5, 0)) !=
            chess::kPieceRook) {
      failed = true;
    }
  }

  {
    chess::Game game("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1");
    game.make_move(chess::Move(chess::Square(4, 0), chess::Square(2, 0)));

    auto board = game.board();
    if (board.piece_type_at(chess::kSideWhite, chess::Square(2, 0)) !=
            chess::kPieceKing ||
        board.piece_type_at(chess::kSideWhite, chess::Square(3, 0)) !=
            chess::kPieceRook) {
      failed = true;
    }
  }

  return failed;
}

bool test_promotion() {
  bool failed = false;

  {
    chess::Game game("7k/P7/8/8/8/8/8/7K w - - 0 1");
    game.make_move(chess::Move(chess::Square(0, 6), chess::Square(0, 7), false,
                               false, chess::kPromoteQueen));

    auto board = game.board();
    if (board.piece_type_at(chess::kSideWhite, chess::Square(0, 7)) !=
        chess::kPieceQueen) {
      failed = true;
    }
  }

  {
    chess::Game game("1q5k/P7/8/8/8/8/8/7K w - - 0 1");
    game.make_move(chess::Move(chess::Square(0, 6), chess::Square(1, 7), true,
                               false, chess::kPromoteRook));

    auto board = game.board();
    if (board.piece_type_at(chess::kSideWhite, chess::Square(1, 7)) !=
        chess::kPieceRook) {
      failed = true;
    }
  }

  return failed;
}

bool test_en_passant_capture() {
  bool failed = false;

  {
    chess::Game game(
        "rnbqkbnr/pppp1ppp/8/3Pp3/8/8/PPP1PPPP/RNBQKBNR w KQkq e6 0 2");
    game.make_move(
        chess::Move(chess::Square(3, 4), chess::Square(4, 5), true, true));

    auto board = game.board();
    if (board.piece_type_at(chess::kSideBlack, chess::Square(4, 4)) ==
        chess::kPiecePawn) {
      failed = true;
    }
  }

  return failed;
}

int main() {
  bool failed = false;

  if (test_fen()) {
    std::cerr << "FEN test failed" << std::endl;
    failed = true;
  }

  if (test_castling_rights()) {
    std::cerr << "Castling rights test failed" << std::endl;
    failed = true;
  }

  if (test_castling()) {
    std::cerr << "Castling move test failed" << std::endl;
    failed = true;
  }

  if (test_promotion()) {
    std::cerr << "Promotion move test failed" << std::endl;
    failed = true;
  }

  if (test_en_passant_capture()) {
    std::cerr << "En passant capture test failed" << std::endl;
    failed = true;
  }

  return failed;
}