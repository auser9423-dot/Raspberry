#ifndef GENERATE
#define GENERATE

#include "board.hpp"
#include "pieces.hpp"
#include <array>
#include <cmath>

// Constants
static constexpr int max_moves{ 218 };
static constexpr std::array<int, 8> knight_offsets{ -25, -23, -14, -10, 10, 14, 23, 25 };
static constexpr std::array<int, 8> king_offsets{ -13, -12, -11, -1, 1, 11, 12, 13 };
static constexpr std::array<int, 4> diagonal_offsets{ -13, -11, 11, 13 };
static constexpr std::array<int, 4> straight_offsets{ -12, -1, 1, 12 };

// Pawn movement
static constexpr int single_push{ 12 };
static constexpr int double_push{ single_push * 2 };
static constexpr int left_capture{ 13 };
static constexpr int right_capture{ 11 };

struct Move
{
    int start{};
    int end{};
    int promotion_piece{};
};

struct Moves
{
    int move_count{};
    std::array<Move, max_moves> moves;
};

Moves generate_pseudo_moves(const Board& board, int colour)
{
    int move_count{};
    std::array<Move, max_moves> moves;

    for (int start{ board_start }; start <= board_end; start++)
    {
        if (board.board[start] == out_of_bounds || board.board[start] * colour <= 0)
        {
            continue;
        }

        int piece_type{ std::abs(board.board[start]) };
        if (piece_type == pawn)
        {
            // Single push
            int single_push_end{ (start + single_push) * colour };
            int single_push_destination{ board.board[single_push_end] };
            if (single_push_destination == empty)
            {
                moves[move_count] = {start, single_push_end, none};
                move_count++;

                // Double push
                int double_push_end{ (start + double_push) * colour };
                int double_push_destination{ board.board[double_push_end] };
                if (double_push_destination == empty)
                {
                    moves[move_count] = {start, double_push_end, none};
                    move_count++;
                }
            }

        }
        else if (piece_type == knight)
        {
            for (int i{}; i < knight_offsets.size(); i++)
            {
                int end{ start + knight_offsets[i] };
                int destination{ board.board[end] };
                if (destination != out_of_bounds && destination * colour <= 0)
                {
                    moves[move_count] = {start, end, none};
                    move_count++;
                }
            }
        }
        else if (piece_type == king)
        {
            for (int i{}; i < king_offsets.size(); i++)
            {
                int end{ start + king_offsets[i] };
                int destination{ board.board[end] };
                if (destination != out_of_bounds && destination * colour <= 0)
                {
                    moves[move_count] = {start, end, none};
                    move_count++;
                }
            }
        }
        else if (piece_type == bishop || piece_type == queen)
        {
            for (int i{}; i < diagonal_offsets.size(); i++)
            {
                int end{ start + diagonal_offsets[i] };
                int destination{ board.board[end] };
                while (destination != out_of_bounds && destination * colour <= 0)
                {
                    moves[move_count] = {start, end, none};
                    move_count++;

                    end += diagonal_offsets[i];
                    destination = board.board[end];
                }
            }
        }

        if (piece_type == rook || piece_type == queen)
        {
            for (int i{}; i < straight_offsets.size(); i++)
            {
                int end{ start + straight_offsets[i] };
                int destination{ board.board[end] };
                while (destination != out_of_bounds && destination * colour <= 0)
                {
                    moves[move_count] = {start, end, none};
                    move_count++;

                    end += straight_offsets[i];
                    destination = board.board[end];
                }
            }
        }
    }

    return {move_count, moves};
}

#endif