#ifndef BOARD_HELPERS
#define BOARD_HELPERS

class Board;

struct Moves;
struct Move;
struct History;
struct NullHistory;

Moves generate_pseudo_moves(const Board& board, int colour);
Moves generate_legal_moves(Board& board, int colour);
bool is_square_attacked(const Board& board, int square, int attacking_colour);
History make_move(Board& board, const Move& move);
void undo_move(Board& board, const Move& move, const History& history);
int evaluate (const Board& board, int colour);
int negamax(Board& board, int alpha, int beta, int colour, int depth, bool is_null_move);
NullHistory make_null_move(Board& board);
void undo_null_move(Board& board, NullHistory& null_history);

#endif