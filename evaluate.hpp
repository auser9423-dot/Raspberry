#ifndef EVALUATE
#define EVALUATE

#include "board.hpp"
#include "pieces.hpp"
#include "bitboard.hpp"
#include "PSTs.hpp"
#include "colour.hpp"
#include "move.hpp"
#include <array>
#include <bit>
#include <algorithm>
#include <cmath>

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

inline static constexpr std::array<std::array<int, 6>, 6> MVV_LVA
{
    { // p,   n,   b,   r,   q,   k   (attacker)
        {600, 500, 400, 300, 200, 100}, // p   (victim)
        {700, 600, 500, 400, 300, 200}, // n
        {800, 700, 600, 500, 400, 300}, // b
        {900, 800, 700, 600, 500, 400}, // r
        {1000, 900, 800, 700, 600, 500}, // q
        {1100, 1000, 900, 800, 700, 600} // k
    }
};

void order_moves(Moves& moves)
{
    std::sort(moves.moves.begin(), moves.moves.begin() + moves.move_count, [](Move a, Move b)
    {
        int score_a = (a.captured_piece == empty) ? 0 : MVV_LVA[std::abs(a.captured_piece) - 1][std::abs(a.piece) - 1];
        int score_b = (b.captured_piece == empty) ? 0 : MVV_LVA[std::abs(b.captured_piece) - 1][std::abs(b.piece) - 1];
        return score_a > score_b;
    });
}

int evaluate (const Board& board, int colour)
{
    int evaluation{};
    for (size_t i{}; i < pieces.size(); i++)
    {
        int piece{ pieces[i] };
        int piece_colour = (piece > 0) ? white : black;

        // Material evaluation
        evaluation += piece_values[piece + piece_value_offset] * std::popcount(board.bitboards.bitboards[piece + bitboard_offset]);

        // Piece-square table evaluations
        PST mg_table{ mg_PSTs[piece + table_offset] };
        Bitboard piece_bitboard{ board.bitboards.bitboards[piece + bitboard_offset] };
        while (piece_bitboard)
        {
            int index{ std::countr_zero(piece_bitboard) };

            evaluation += mg_table[index] * piece_colour;
            piece_bitboard ^= (1ULL << index);
        }
    }
    
    return evaluation * colour;
}

#endif