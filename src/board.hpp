#ifndef BOARD
#define BOARD

#include "board_helpers.hpp"
#include "pieces.hpp"
#include "bitboard.hpp"
#include <array>
#include <string>

// Constants
inline static constexpr int board_size{ 144 };
inline static constexpr int board_start{ 26 };
inline static constexpr int board_end{ 117 };
inline static constexpr int rows{ 12 };
inline static constexpr int columns{ 12 };
inline static constexpr int out_of_bounds{ -9 };
inline static constexpr int empty{};
inline static constexpr int no_en_passant{ out_of_bounds };
inline static constexpr std::array<int, board_size> board_144_to_64 = []()
{
    std::array<int, board_size> temp_board{};
    int index_counter{};

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
            temp_board[i] = index_counter;
            index_counter++;
        }
    }
    return temp_board;
}();

inline int square_to_index(std::string square)
{
    int column{};
    int row{ (square[1] - '0') + 1 };

    switch (square[0])
    {
        case 'a':
            column = 2;
            break;
        case 'b':
            column = 3;
            break;
        case 'c':
            column = 4;
            break;
        case 'd':
            column = 5;
            break;
        case 'e':
            column = 6;
            break;
        case 'f':
            column = 7;
            break;
        case 'g':
            column = 8;
            break;
        case 'h':
            column = 9;
            break;
    }

    return (row * columns) + column;
}

inline static const std::array<std::string, board_size> index_to_square = []()
{
    std::array<std::string, board_size> temp_board{};
    for (int i{}; i < board_size; i++)
    {
        if
        (
            i % columns == 0 || i % columns == 1 || i % columns == 10 || i % columns == 11
            || i / rows == 0 || i / rows == 1 || i / rows == 10 || i / rows == 11
        )
        {
            temp_board[i] = " ";
        }
        else
        {
            char column{};
            switch ((i % columns) - 1)
            {
                case 1:
                    column = 'a';
                    break;
                case 2:
                    column = 'b';
                    break;
                case 3:
                    column = 'c';
                    break;
                case 4:
                    column = 'd';
                    break;
                case 5:
                    column = 'e';
                    break;
                case 6:
                    column = 'f';
                    break;
                case 7:
                    column = 'g';
                    break;
                case 8:
                    column = 'h';
                    break;
                default:
                    column = 'x';
                    break;
            }

            char row{ static_cast<char>(((i / rows) - 1) + '0') };
            temp_board[i] = {column, row};
        }
    }
    return temp_board;
}();

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

    Bitboards bitboards{};

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

    int current_material{ (piece_values[queen + piece_value_offset] * 2) + (piece_values[rook + piece_value_offset] * 4) + (piece_values[bishop + piece_value_offset] * 4) + (piece_values[knight + piece_value_offset] * 4) };

    static constexpr std::array<int, 8> default_piece_order{rook, knight, bishop, queen, king, bishop, knight, rook};

    friend Moves generate_pseudo_moves(const Board& board, int colour);
    friend Moves generate_legal_moves(Board& board, int colour);
    friend bool is_square_attacked(const Board& board, int square, int attacking_colour);
    friend History make_move(Board& board, const Move& move);
    friend void undo_move(Board& board, const Move& move, const History& history);
    friend int evaluate (const Board& board, int colour);

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
                    bitboards.bitboards[board[i] + bitboard_offset] |= (1ULL << board_144_to_64[i]);
                }
                else if (row == 3)
                {
                    board[i] = w_pawn;
                    bitboards.bitboards[board[i] + bitboard_offset] |= (1ULL << board_144_to_64[i]);
                }
                else if (row == 8)
                {
                    board[i] = b_pawn;
                    bitboards.bitboards[board[i] + bitboard_offset] |= (1ULL << board_144_to_64[i]);
                }
                else if (row == 9)
                {
                    board[i] = -default_piece_order[column - 2];
                    bitboards.bitboards[board[i] + bitboard_offset] |= (1ULL << board_144_to_64[i]);
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