#ifndef BOARD
#define BOARD

#include "board_helpers.hpp"
#include <array>

// Constants
static constexpr int board_size{ 144 };
static constexpr int board_start{ 26 };
static constexpr int board_end{ 117 };
static constexpr int rows{ 12 };
static constexpr int columns{ 12 };
static constexpr int out_of_bounds{ -9 };
static constexpr int empty{};

class Board
{
    private:
    std::array<int, board_size> board = []()
    {
        std::array<int, board_size> temp_board{};
        for (int i{}; i < board_size; i++)
        {
            if
            (
                i % columns == 0 || i % columns == 1 || i % columns == 10 || i % columns == 11
                || i / rows == 0 || i / rows == 1 || i / rows == 10 || i / rows == 11
            )
            {
                temp_board[i] = out_of_bounds;
            }
        }

        return temp_board;
    }();

    friend Moves generate_pseudo_moves(const Board& board, int colour);
};

#endif