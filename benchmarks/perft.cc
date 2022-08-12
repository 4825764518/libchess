#include <libchess/game.h>
#include <libchess/move_generator.h>
#include <iostream>

int perft(chess::Game& game, int depth) {
	if (depth == 0) {
		return 1;
	}

	chess::MoveGenerator move_generator;
	chess::MoveList moves = move_generator.generate_legal_moves(game);
	int count = 0;	

	for (auto i = 0; i < moves.size(); i++) {
		auto move = moves.moves()[i];
		auto new_game = game;
		new_game.make_move(move);
		count += perft(new_game, depth - 1);
		new_game.unmake_move();
	}

	return count;
}

int main() {
	constexpr int depth = 3;

	chess::Game game;
	auto count = perft(game, depth);
	std::cout << "Count: " << count << std::endl;
}