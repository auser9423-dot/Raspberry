#ifndef MOVE
#define MOVE

#include "board.hpp"
#include <array>

// Constants 
static constexpr int max_moves{ 218 };

// Move types
static constexpr int castle_move{};
static constexpr int double_push_move{ 1 };
static constexpr int promotion_move{ 2 };
static constexpr int capture_promotion_move{ 3 };

struct Move
{
    int start{};
    int end{};
    int promotion_piece{};
    int move_type{};
    int piece{};
    int captured_piece{};
};

struct Moves
{
    int move_count{};
    std::array<Move, max_moves> moves;
};

void make_move(Board& board, const Move& move)
{
    
}

#endif