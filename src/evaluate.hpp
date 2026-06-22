#ifndef EVALUATE
#define EVALUATE

#include "board.hpp"
#include "pieces.hpp"
#include "bitboard.hpp"
#include "PSTs.hpp"
#include "colour.hpp"
#include "move.hpp"
#include "bitmasks.hpp"
#include <array>
#include <bit>
#include <algorithm>
#include <cmath>

// Constants
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

inline static constexpr int start_material{ (piece_values[queen + piece_value_offset] * 2) + (piece_values[rook + piece_value_offset] * 4) + (piece_values[bishop + piece_value_offset] * 4) + (piece_values[knight + piece_value_offset] * 4) };

void order_moves(Moves& moves, Move& hash_move)
{
    for (int i{}; i < moves.move_count; i++)
    {
        Move& move{ moves.moves[i] };
        if (move.piece != none && move.start == hash_move.start && move.end == hash_move.end && move.promotion_piece == hash_move.promotion_piece)
        {
            move.score = 1000000;
        }
        else if (move.move_type == en_passant_move)
        {
            move.score = MVV_LVA[0][0];
        }
        else if (move.captured_piece != none)
        {
            move.score = (move.captured_piece == empty) ? 0 : MVV_LVA[std::abs(move.captured_piece) - 1][std::abs(move.piece) - 1];
        }
    }

    std::sort(moves.moves.begin(), moves.moves.begin() + moves.move_count, [](const Move& a, const Move& b)
    {
        return a.score > b.score;
    });
}

int evaluate (const Board& board, int colour)
{
    int evaluation{};
    double ratio{ static_cast<double>(board.current_material) / start_material };
    ratio = std::clamp(ratio, 0.0, 1.0);

    for (size_t i{}; i < pieces.size(); i++)
    {
        int piece{ pieces[i] };
        if (piece != empty)
        {
            int piece_colour = (piece > 0) ? white : black;

            // Material evaluation
            evaluation += piece_values[piece + piece_value_offset] * std::popcount(board.bitboards.bitboards[piece + bitboard_offset]);

            // Piece-square table evaluations
            PST mg_table{ mg_PSTs[piece + table_offset] };
            PST eg_table{ eg_PSTs[piece + table_offset] };
            Bitboard piece_bitboard{ board.bitboards.bitboards[piece + bitboard_offset] };
            while (piece_bitboard)
            {
                int index{ std::countr_zero(piece_bitboard) };

                evaluation += ((mg_table[index] * ratio) + (eg_table[index] * (1 - ratio))) * piece_colour;
                piece_bitboard ^= (1ULL << index);
            }

            if (std::abs(piece) == pawn)
            {
                std::array<uint64_t, 64> passed_pawn_mask = (piece_colour == white) ? w_passed_pawn_mask : b_passed_pawn_mask;

                Bitboard pawn_bitboard{ board.bitboards.bitboards[piece + bitboard_offset] };
                while (pawn_bitboard)
                {
                    // Passed pawn bonus
                    int index{ std::countr_zero(pawn_bitboard) };
                    if ((passed_pawn_mask[index] & board.bitboards.bitboards[-piece + bitboard_offset]) == 0)
                    {
                        evaluation += 35 * piece_colour;
                    }

                    pawn_bitboard ^= (1ULL << index);
                }

                // Doubled pawn punishment
                pawn_bitboard = board.bitboards.bitboards[piece + bitboard_offset];
                for (int j{}; j < 8; j++)
                {
                    int stacked_pawns{ std::popcount(stacked_pawns_mask[j] & pawn_bitboard) };
                    if (stacked_pawns > 1)
                    {
                        evaluation -= 15 * stacked_pawns * piece_colour;
                    }
                }
            }
        }
    }
    
    return evaluation * colour;
}

#endif