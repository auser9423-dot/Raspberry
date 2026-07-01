#ifndef BOARD_HELPERS
#define BOARD_HELPERS

#include <cstdint>

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
int negamax(Board& board, int alpha, int beta, int colour, int depth, int ply, bool is_null_move, uint64_t& nodes);
NullHistory make_null_move(Board& board);
void undo_null_move(Board& board, NullHistory& null_history);
Move search(Board& board, int alpha, int beta, int colour, int depth);
int quiescence(Board& board, int alpha, int beta, int colour, uint64_t& nodes);
void order_moves(Board& board, Moves& moves, Move& hash_move, int colour, int ply);

#endif