#ifndef MOVE
#define MOVE

#include "bitboard.hpp"
#include "colour.hpp"
#include "pieces.hpp"
#include <array>
#include <cmath>
#include <cstdint>

class Board;

// Constants 
inline static constexpr int max_moves{ 218 };

// Move types
inline static constexpr int en_passant_move{ 6 };
inline static constexpr int double_push_move{ 1 };
inline static constexpr int promotion_move{ 2 };
inline static constexpr int capture_promotion_move{ 3 };
inline static constexpr int king_side_castle_move{ 4 };
inline static constexpr int queen_side_castle_move{ 5 };

struct Move
{
    int start{};
    int end{};
    int promotion_piece{};
    int move_type{};
    int piece{};
    int captured_piece{};
    bool is_legal{};
    int score{};
};

struct Moves
{
    int move_count{};
    std::array<Move, max_moves> moves;
};

struct History
{
    int en_passant_square{};
    bool white_king_rook_moved{};
    bool white_queen_rook_moved{};
    bool black_king_rook_moved{};
    bool black_queen_rook_moved{};
    int white_king_position{};
    int black_king_position{};
    bool white_king_moved{};
    bool black_king_moved{};
    uint64_t zobrist_position{};
    int moves_since_pawn_move{};
};

struct NullHistory
{
    int en_passant_square{};
    uint64_t zobrist_position{};
};


History make_move(Board& board, const Move& move);
void undo_move(Board& board, const Move& move, const History& history);
NullHistory make_null_move(Board& board);
void undo_null_move(Board& board, NullHistory& null_history);

#endif