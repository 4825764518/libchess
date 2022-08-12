#include <bitset>
#include <iostream>

#include <libchess/board.h>

int main() {
  bool failed = false;
  {
    auto board = chess::Board::from_fen("");
    if (board != chess::null_board) {
      std::cerr << "Empty FEN didn't return null board" << std::endl;
      failed = true;
    }
  }

  {
    auto board = chess::Board::from_fen("8/8/8/8/8/8/8/P7 b KQq h8 0 1");
    if (board.pawns(0) != 1) {
      std::cerr << "Single pawn test failed" << std::endl;
      failed = true;
    }

    if (board.turn() != 1) {
      std::cerr << "FEN turn test failed" << std::endl;
      failed = true;
    }

    if (board.castling(chess::kCastleBlackKingSide) != 0) {
      std::cerr << "Castle rights test failed" << std::endl;
      failed = true;
    }

    if (board.ep_square() != 63) {
      std::cerr << "En passant square test failed" << std::endl;
      failed = true;
    }
  }

  {
    auto board =
        chess::Board::from_fen("8/pppppppp/8/8/8/8/PPPPPPPP/8 w - - 0 1");
    auto white_pawns = board.pawns(chess::kSideWhite).data();
    auto black_pawns = board.pawns(chess::kSideBlack).data();
    if (white_pawns != 0x000000000000ff00 ||
        black_pawns != 0x00ff000000000000) {
      std::cerr << "Pawn test failed" << std::endl;
      std::cerr << "  White pawns: " << std::hex << white_pawns << " - "
                << std::bitset<64>(white_pawns) << std::endl;
      std::cerr << "  Black pawns: " << std::hex << black_pawns << " - "
                << std::bitset<64>(black_pawns) << std::endl;
      failed = true;
    }
  }

  return failed;
}