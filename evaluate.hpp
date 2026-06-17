#ifndef EVALUATE
#define EVALUATE

#include "board.hpp"
#include "pieces.hpp"
#include "bitboard.hpp"
#include <array>
#include <bit>

// Constants
inline static constexpr std::array<int, 13> piece_values
{
    // b_king, b_queen, b_rook, b_bishop, b_knight, b_pawn, garbage
       -10000, -1000,   -525,   -350,     -325,     -100,   0,
    // w_pawn, w_knight, w_bishop, w_rook, w_queen, w_king
       100,    325,      350,      525,    1000,    10000
};

inline static constexpr int piece_value_offset{ 6 };
inline static constexpr std::array<int, 13> pieces
{
    b_king, b_queen, b_rook, b_bishop, b_knight, b_pawn, empty, // empty = garbage
    w_pawn, w_knight, w_bishop, w_rook, w_queen, w_king,
};

int evaluate (const Board& board, int colour)
{
    int evaluation{};
    for (size_t i{}; i < pieces.size(); i++) // Material evaluation
    {
        int piece{ pieces[i] };
        evaluation += piece_values[piece + piece_value_offset] * std::popcount(board.bitboards.bitboards[piece + bitboard_offset]);
    }

    // Piece-square table evaluations
    
    return evaluation * colour;
}

#endif