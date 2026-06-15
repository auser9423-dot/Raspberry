#ifndef GENERATE
#define GENERATE

#include "board.hpp"
#include <array>

// Constants
static constexpr int max_moves{ 218 };
static constexpr std::array<int, 8> knight_offsets{ -25, -23, -14, -10, 10, 14, 23, 25 };

std::array<int, max_moves> generate_pseudo_moves(const Board& board, int colour)
{

}

#endif