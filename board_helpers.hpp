#ifndef BOARD_HELPERS
#define BOARD_HELPERS

class Board;

struct Moves;
struct Move;

Moves generate_pseudo_moves(const Board& board, int colour);
bool is_square_attacked(const Board& board, int square, int attacking_colour);
void make_move(Board& board, const Move& move);

#endif