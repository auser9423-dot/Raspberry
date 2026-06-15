#ifndef BOARD_HELPERS
#define BOARD_HELPERS

class Board;

struct Moves;

Moves generate_pseudo_moves(const Board& board, int colour);

#endif