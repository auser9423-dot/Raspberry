#ifndef BOARD_HELPERS
#define BOARD_HELPERS

class Board;

struct Moves;
struct Move;
struct History;

Moves generate_pseudo_moves(const Board& board, int colour);
Moves generate_legal_moves(Board& board, int colour);
bool is_square_attacked(const Board& board, int square, int attacking_colour);
History make_move(Board& board, const Move& move);
void undo_move(Board& board, const Move& move, const History& history);

#endif