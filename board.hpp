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
static constexpr int no_en_passant{ out_of_bounds };

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

    // Board state variables
    int en_passant_square{ no_en_passant };
    bool white_king_rook_moved{ false };
    bool white_queen_rook_moved{ false };
    bool black_king_rook_moved{ false };
    bool black_queen_rook_moved{ false };
    int white_king_position{ board_start + 4 };
    int black_king_position{ board_end - 3 };
    bool white_king_moved{ false };
    bool black_king_moved{ false };


    friend Moves generate_pseudo_moves(const Board& board, int colour);
    friend Moves generate_legal_moves(Board& board, int colour);
    friend bool is_square_attacked(const Board& board, int square, int attacking_colour);
    friend History make_move(Board& board, const Move& move);
    friend void undo_move(Board& board, const Move& move, const History& history);

    public:
};

#endif