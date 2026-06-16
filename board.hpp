#ifndef BOARD
#define BOARD

#include "board_helpers.hpp"
#include "pieces.hpp"
#include <array>

// Constants
inline static constexpr int board_size{ 144 };
inline static constexpr int board_start{ 26 };
inline static constexpr int board_end{ 117 };
inline static constexpr int rows{ 12 };
inline static constexpr int columns{ 12 };
inline static constexpr int out_of_bounds{ -9 };
inline static constexpr int empty{};
inline static constexpr int no_en_passant{ out_of_bounds };

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
            else
            {
                temp_board[i] = empty;
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

    static constexpr std::array<int, 8> default_piece_order{rook, knight, bishop, queen, king, bishop, knight, rook};

    friend Moves generate_pseudo_moves(const Board& board, int colour);
    friend Moves generate_legal_moves(Board& board, int colour);
    friend bool is_square_attacked(const Board& board, int square, int attacking_colour);
    friend History make_move(Board& board, const Move& move);
    friend void undo_move(Board& board, const Move& move, const History& history);

    public:
    void setup_default_start_position()
    {
        for (int i{ board_start }; i <= board_end; i++)
        {
            int column{ i % columns };
            if (column != 0 && column != 1 && column != 10 && column != 11)
            {
                int row{ i / rows };

                if (row == 2)
                {
                    board[i] = default_piece_order[column - 2];
                }
                else if (row == 3)
                {
                    board[i] = w_pawn;
                }
                else if (row == 8)
                {
                    board[i] = b_pawn;
                }
                else if (row == 9)
                {
                    board[i] = -default_piece_order[column - 2];
                }
                else
                {
                    board[i] = empty;
                }
            }
        }
    }
};

#endif