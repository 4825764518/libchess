#include <cstring>
#include <iostream>
#include <regex>
#include <sstream>

#include <libchess/bitboard_iterator.h>
#include <libchess/board.h>
#include <libchess/piece.h>

namespace chess {
Board null_board({0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {1, 1, 1, 1},
                 0, -1, 0, 0);

inline int char_to_int(char ch) { return ch - 48; }

inline int square_name_to_offset(std::string square_name) {
  auto file_offset = static_cast<int>(square_name.at(0) - 'a');
  auto rank_offset = static_cast<int>(square_name.at(1) - '1') * 8;
  auto square = file_offset + rank_offset;
  return square;
}

inline std::string square_index_to_name(Square square) {
  std::stringstream stream;
  stream << static_cast<char>(square.file() + 'a');
  stream << static_cast<char>(square.rank() + '1');
  return stream.str();
}

Board::Board(std::vector<Bitboard> pawns, std::vector<Bitboard> knights,
             std::vector<Bitboard> bishops, std::vector<Bitboard> rooks,
             std::vector<Bitboard> queens, std::vector<Bitboard> kings,
             std::vector<bool> castling, int side, Square ep_square,
             int half_move, int full_move)
    : side_(side), half_move_(half_move), full_move_(full_move),
      ep_square_(ep_square) {
  for (auto i = 0; i < kNumSides; i++) {
    this->pieces_[i][kPiecePawn] = pawns[i];
    this->pieces_[i][kPieceKnight] = knights[i];
    this->pieces_[i][kPieceBishop] = bishops[i];
    this->pieces_[i][kPieceRook] = rooks[i];
    this->pieces_[i][kPieceQueen] = queens[i];
    this->pieces_[i][kPieceKing] = kings[i];
  }

  for (auto i = 0; i < kNumCastle; i++) {
    this->castling_[i] = castling[i];
  }

  this->update_occupied();
}

bool Board::operator==(const Board &board) const {
  for (auto side = 0; side < 2; side++) {
    if (this->pawns(side) != board.pawns(side)) {
      return false;
    }
    if (this->knights(side) != board.knights(side)) {
      return false;
    }
    if (this->bishops(side) != board.bishops(side)) {
      return false;
    }
    if (this->rooks(side) != board.rooks(side)) {
      return false;
    }
    if (this->queens(side) != board.queens(side)) {
      return false;
    }
    if (this->kings(side) != board.kings(side)) {
      return false;
    }

    for (auto castle_side = 0; castle_side < kNumCastle; castle_side++) {
      if (this->castling(castle_side) != board.castling(castle_side)) {
        return false;
      }
    }
  }

  if (this->turn() != board.turn()) {
    return false;
  }

  if (this->ep_square() != board.ep_square()) {
    return false;
  }

  if (this->half_move() != board.half_move()) {
    return false;
  }

  if (this->full_move() != board.full_move()) {
    return false;
  }

  return true;
}

Board Board::from_fen(std::string fen) {
  // FEN strings look like this:
  // rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1
  // rnbq1bnr/pp2k1pp/5p2/1Bp1N3/3p2P1/4P2P/PPPP1P2/RNBQK2R w KQ - 2 7
  // 1r4nn/p4k1r/3P3b/4RppP/1P6/P1NR1B1P/2PB1P2/6K1 b - - 4 34
  std::vector<Bitboard> pawns = {0, 0};
  std::vector<Bitboard> knights = {0, 0};
  std::vector<Bitboard> bishops = {0, 0};
  std::vector<Bitboard> rooks = {0, 0};
  std::vector<Bitboard> queens = {0, 0};
  std::vector<Bitboard> kings = {0, 0};
  std::vector<bool> castling = {false, false, false, false};
  int side = 0;
  int ep_square = -1;
  int halfmove = 0;
  int fullmove = 0;

  std::regex fen_exp("(.+) (.) (.+) (.+) (.+) (.+)");
  std::smatch fen_match;
  if (!std::regex_search(fen, fen_match, fen_exp)) {
    return null_board;
  }

  if (fen_match.length() < 7) {
    return null_board;
  }

  auto fen_pieces = fen_match[1].str();
  {
    std::vector<std::string> ranks = {};
    // TODO: use a c++ string equivalent here. strtok is used because the
    // current solution that exists in c++ is awful.
    {
      char token_str[1024] = {'\0'};
      fen_pieces.copy(token_str, 1024, 0);
      char *token = std::strtok(token_str, "/");
      while (ranks.size() < 8) {
        ranks.push_back(token);
        token = std::strtok(NULL, "/");
      }
    }

    for (auto i = 0; i < 8; i++) {
      // Ranks in FEN format are from highest to lowest
      auto rank = 7 - i;
      auto rank_str = ranks[i];
      // Offset of the current rank into a bitboard
      auto rank_offset = 8 * rank;
      // Number of tiles filled in the current rank
      int rank_filled = 0;
      for (char &ch : rank_str) {
        if (std::isdigit(ch)) {
          auto num = char_to_int(ch);
          rank_filled += num;
        } else {
          int piece_side = std::isupper(ch) ? 0 : 1;
          auto piece_type = std::tolower(ch);
          Bitboard board = 0;
          switch (piece_type) {
          case 'p':
            board = pawns[piece_side];
            break;
          case 'n':
            board = knights[piece_side];
            break;
          case 'b':
            board = bishops[piece_side];
            break;
          case 'r':
            board = rooks[piece_side];
            break;
          case 'q':
            board = queens[piece_side];
            break;
          case 'k':
            board = kings[piece_side];
            break;
          default:
            break;
          }

          board.set(Square(rank_offset + rank_filled));
          rank_filled++;

          switch (piece_type) {
          case 'p':
            pawns[piece_side] = board;
            break;
          case 'n':
            knights[piece_side] = board;
            break;
          case 'b':
            bishops[piece_side] = board;
            break;
          case 'r':
            rooks[piece_side] = board;
            break;
          case 'q':
            queens[piece_side] = board;
            break;
          case 'k':
            kings[piece_side] = board;
            break;
          default:
            break;
          }
        }
      }
    }
  }

  auto fen_side = fen_match[2].str();
  { side = fen_side.compare("w") == 0 ? kSideWhite : kSideBlack; }

  auto fen_castling = fen_match[3].str();
  {
    if (fen_castling.compare("-") != 0) {
      if (fen_castling.find("K") != std::string::npos) {
        castling[kCastleWhiteKingSide] = true;
      }

      if (fen_castling.find("Q") != std::string::npos) {
        castling[kCastleWhiteQueenSide] = true;
      }

      if (fen_castling.find("k") != std::string::npos) {
        castling[kCastleBlackKingSide] = true;
      }

      if (fen_castling.find("q") != std::string::npos) {
        castling[kCastleBlackQueenSide] = true;
      }
    }
  }

  auto fen_ep = fen_match[4].str();
  {
    if (fen_ep.compare("-") != 0) {
      ep_square = square_name_to_offset(fen_ep);
    }
  }

  auto fen_halfmove = fen_match[5].str();
  halfmove = std::stoi(fen_halfmove);

  auto fen_fullmove = fen_match[6].str();
  fullmove = std::stoi(fen_fullmove);

  return Board(pawns, knights, bishops, rooks, queens, kings, castling, side,
               ep_square, halfmove, fullmove);
}

std::string Board::fen() const {
  const char piece_symbols[] = {
      'p', 'n', 'b', 'r', 'q', 'k',
  };

  const char turn_symbols[] = {'w', 'b'};

  std::stringstream stream;

  for (auto rank = 7; rank >= 0; --rank) {
    if (rank < 7) {
      stream << "/";
    }

    int empty = 0;
    for (auto file = 0; file < 8; file++) {
      auto square = Square(file, rank);
      const auto white_piece = piece_type_at(kSideWhite, square);
      const auto black_piece = piece_type_at(kSideBlack, square);
      if (white_piece == -1 && black_piece == -1) {
        empty++;
        continue;
      }

      if (empty) {
        stream << empty;
        empty = 0;
      }

      auto symbol = '\0';
      if (white_piece >= kPiecePawn) {
        symbol = std::toupper(piece_symbols[white_piece]);
      } else {
        symbol = piece_symbols[black_piece];
      }

      stream << symbol;
    }

    if (empty) {
      stream << empty;
      empty = 0;
    }
  }

  stream << " " << turn_symbols[turn()];

  {
    // TODO: clean this up
    const char castle_symbols[] = {'K', 'Q', 'k', 'q'};

    stream << " ";

    bool can_castle = false;
    for (int castle_side = 0; castle_side < kNumCastle; castle_side++) {
      if (castling(castle_side)) {
        can_castle = true;
        stream << castle_symbols[castle_side];
      }
    }

    if (!can_castle) {
      stream << "-";
    }
  }

  {
    // TODO: clean this up
    auto square = ep_square();
    if (square == null_square) {
      stream << " -";
    } else {
      stream << " " << square_index_to_name(square);
    }
  }

  stream << " " << half_move();
  stream << " " << full_move();

  return stream.str();
}

uint64_t Board::attacks_to_square(int side, Square square) const {
  const auto pawn_board = piece_board(side, kPiecePawn);
  const auto pawn_attacks = pawn_attack_board(side, pawn_board);

  const auto knight_board = piece_board(side, kPieceKnight);
  const auto knight_attacks = knight_attack_board(knight_board);

  const auto bishop_board = piece_board(side, kPieceBishop);
  const auto bishop_attacks = bishop_attack_board(occupied(), bishop_board);

  const auto rook_board = piece_board(side, kPieceRook);
  const auto rook_attacks = rook_attack_board(occupied(), rook_board);

  const auto queen_board = piece_board(side, kPieceQueen);
  const auto queen_attacks = queen_attack_board(occupied(), queen_board);

  const auto king_board = piece_board(side, kPieceKing);
  const auto king_attacks = king_attack_board(king_board);

  return pawn_attacks.occupied(square) | knight_attacks.occupied(square) |
         bishop_attacks.occupied(square) | rook_attacks.occupied(square) |
         queen_attacks.occupied(square) | king_attacks.occupied(square);
}

bool Board::check(int side) const {
  auto kings = this->kings(side);
  BitboardIterator king_iter(kings);
  while (king_iter.has_data()) {
    auto square = king_iter.next();
    return attacks_to_square(!side, square);
  }

  return false;
}

} // namespace chess