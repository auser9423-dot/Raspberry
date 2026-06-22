#ifndef BOARD
#define BOARD

#include "board_helpers.hpp"
#include "pieces.hpp"
#include "bitboard.hpp"
#include "transposition_table.hpp"
#include <array>
#include <string>
#include <random>
#include <cmath>

// Constants
inline static constexpr int white_king_side_castle_right{};
inline static constexpr int white_queen_side_castle_right{ 1 };
inline static constexpr int black_king_side_castle_right{ 2 };
inline static constexpr int black_queen_side_castle_right{ 3 };

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

inline std::mt19937_64 gen(12345679ULL);
inline std::uniform_int_distribution<uint64_t> TT_distrib;

inline static const std::array<std::array<uint64_t, board_size>, 13> TT_board = []()
{
    std::array<std::array<uint64_t, board_size>, 13> temp{};
    for (size_t i{}; i < temp.size(); i++)
    {
        for (size_t j{}; j < temp[i].size(); j++)
        {
            temp[i][j] = TT_distrib(gen);
        }
    }

    return temp;
}();

inline int TT_board_offset{ 6 };

inline static const std::array<uint64_t, columns> TT_column = []()
{
    std::array<uint64_t, columns> temp{};
    for (size_t i{}; i < columns; i++)
    {
        temp[i] = TT_distrib(gen);
    }

    return temp;
}();

inline static const uint64_t TT_white{ TT_distrib(gen) };

inline static const std::array<uint64_t, 4> TT_castle_rights = []()
{
    std::array<uint64_t, 4> temp{};
    for (size_t i{}; i < 4; i++)
    {
        temp[i] = TT_distrib(gen);
    }

    return temp;
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
    uint64_t zobrist_position{};

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
    friend NullHistory make_null_move(Board& board);
    friend void undo_null_move(Board& board, NullHistory& null_history);
    friend inline int negamax(Board& board, int alpha, int beta, int colour, int depth, bool is_null_move);

    public:
    void setup_default_start_position()
    {
        zobrist_position = 0;

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

            int piece{ board[i] };
            if (piece != out_of_bounds && piece != empty)
            {
                zobrist_position ^= TT_board[piece + TT_board_offset][i];
            }
        }

        for (int i{}; i < TT_castle_rights.size(); i++)
        {
            zobrist_position ^= TT_castle_rights[i];
        }

        zobrist_position ^= TT_white;
    }
};

inline History make_move(Board& board, const Move& move)
{
    // Save the pervious states
    History history;
    history.en_passant_square = board.en_passant_square;
    history.white_king_rook_moved = board.white_king_rook_moved;
    history.white_queen_rook_moved = board.white_queen_rook_moved;
    history.black_king_rook_moved = board.black_king_rook_moved;
    history.black_queen_rook_moved = board.black_queen_rook_moved;
    history.white_king_position = board.white_king_position;
    history.black_king_position = board.black_king_position;
    history.white_king_moved = board.white_king_moved;
    history.black_king_moved = board.black_king_moved;
    history.zobrist_position = board.zobrist_position;

    board.en_passant_square = no_en_passant;

    if (history.en_passant_square != no_en_passant)
    {
        int en_passant_column{ history.en_passant_square % columns };
        board.zobrist_position ^= TT_column[en_passant_column];
    }

    int colour = (move.piece > 0) ? white : black;

    board.board[move.start] = empty;
    board.bitboards.bitboards[move.piece + bitboard_offset] ^= (1ULL << board_144_to_64[move.start]);
    board.bitboards.bitboards[move.captured_piece + bitboard_offset] ^= (1ULL << board_144_to_64[move.end]);

    board.zobrist_position ^= TT_board[move.piece + TT_board_offset][move.start];

    if (move.captured_piece != none)
    {
        board.zobrist_position ^= TT_board[move.captured_piece + TT_board_offset][move.end];
    }

    board.current_material -= piece_values[std::abs(move.captured_piece) + piece_value_offset];

    if (move.move_type == promotion_move || move.move_type == capture_promotion_move)
    {
        board.board[move.end] = move.promotion_piece;
        board.bitboards.bitboards[move.promotion_piece + bitboard_offset] |= (1ULL << board_144_to_64[move.end]);

        board.zobrist_position ^= TT_board[move.promotion_piece + TT_board_offset][move.end];

        board.current_material -= piece_values[std::abs(move.piece) + piece_value_offset];
        board.current_material += piece_values[std::abs(move.promotion_piece) + piece_value_offset];
    }
    else
    {
        board.board[move.end] = move.piece;
        board.bitboards.bitboards[move.piece + bitboard_offset] |= (1ULL << board_144_to_64[move.end]);

        board.zobrist_position ^= TT_board[move.piece + TT_board_offset][move.end];

        if (move.piece == w_king)
        {
            board.white_king_position = move.end;
        }
        else if (move.piece == b_king)
        {
            board.black_king_position = move.end;
        }

        if (move.move_type == king_side_castle_move)
        {
            if (colour == white)
            {
                board.white_king_moved = true;
                board.white_king_rook_moved = true;

                board.board[board_start + 7] = empty;
                board.board[board_start + 7 - 2] = w_rook;

                board.bitboards.bitboards[w_rook + bitboard_offset] ^= (1ULL << board_144_to_64[board_start + 7]);
                board.bitboards.bitboards[w_rook + bitboard_offset] |= (1ULL << board_144_to_64[board_start + 7 - 2]);

                board.zobrist_position ^= TT_board[w_rook + TT_board_offset][board_start + 7];
                board.zobrist_position ^= TT_board[w_rook + TT_board_offset][board_start + 7 - 2];

                board.white_king_position = move.end;
            }
            else
            {
                board.black_king_moved = true;
                board.black_king_rook_moved = true;

                board.board[board_end] = empty;
                board.board[board_end - 2] = b_rook;

                board.bitboards.bitboards[b_rook + bitboard_offset] ^= (1ULL << board_144_to_64[board_end]);
                board.bitboards.bitboards[b_rook + bitboard_offset] |= (1ULL << board_144_to_64[board_end - 2]);

                board.zobrist_position ^= TT_board[b_rook + TT_board_offset][board_end];
                board.zobrist_position ^= TT_board[b_rook + TT_board_offset][board_end - 2];

                board.black_king_position = move.end;
            }
        }
        else if (move.move_type == queen_side_castle_move)
        {
            if (colour == white)
            {
                board.white_king_moved = true;
                board.white_queen_rook_moved = true;

                board.board[board_start] = empty;
                board.board[board_start + 3] = w_rook;

                board.bitboards.bitboards[w_rook + bitboard_offset] ^= (1ULL << board_144_to_64[board_start]);
                board.bitboards.bitboards[w_rook + bitboard_offset] |= (1ULL << board_144_to_64[board_start + 3]);

                board.zobrist_position ^= TT_board[w_rook + TT_board_offset][board_start];
                board.zobrist_position ^= TT_board[w_rook + TT_board_offset][board_start + 3];

                board.white_king_position = move.end;
            }
            else
            {
                board.black_king_moved = true;
                board.black_queen_rook_moved = true;

                board.board[board_end - 7] = empty;
                board.board[board_end - 7 + 3] = b_rook;

                board.bitboards.bitboards[b_rook + bitboard_offset] ^= (1ULL << board_144_to_64[board_end - 7]);
                board.bitboards.bitboards[b_rook + bitboard_offset] |= (1ULL << board_144_to_64[board_end - 7 + 3]);

                board.zobrist_position ^= TT_board[b_rook + TT_board_offset][board_end - 7];
                board.zobrist_position ^= TT_board[b_rook + TT_board_offset][board_end - 7 + 3];

                board.black_king_position = move.end;
            }
        }
        else if (move.move_type == en_passant_move)
        {
            board.board[move.end + (rows * -colour)] = empty;
            board.bitboards.bitboards[(pawn * -colour) + bitboard_offset] ^= (1ULL << board_144_to_64[move.end + (rows * -colour)]);

            board.zobrist_position ^= TT_board[(pawn * -colour) + TT_board_offset][move.end + (rows * -colour)];

            board.current_material -= piece_values[pawn + piece_value_offset];
        }
        else if (move.move_type == double_push_move)
        {
            board.en_passant_square = move.end + (rows * -colour);

            int en_passant_column{ board.en_passant_square % columns };
            board.zobrist_position ^= TT_column[en_passant_column];
        }
    }

    if (board.white_king_position != board_start + 4)
    {
        if (!board.white_king_moved)
        {
            if (!board.white_king_rook_moved)
            {
                board.zobrist_position ^= TT_castle_rights[white_king_side_castle_right];
            }

            if (!board.white_queen_rook_moved)
            {
                board.zobrist_position ^= TT_castle_rights[white_queen_side_castle_right];
            }
        }

        board.white_king_moved = true;
    }

    if (board.black_king_position != board_end - 3)
    {
        if (!board.black_king_moved)
        {
            if (!board.black_king_rook_moved)
            {
                board.zobrist_position ^= TT_castle_rights[black_king_side_castle_right];
            }

            if (!board.black_queen_rook_moved)
            {
                board.zobrist_position ^= TT_castle_rights[black_queen_side_castle_right];
            }
        }

        board.black_king_moved = true;
    }

    if (board.board[board_start] != w_rook)
    {
        if (!board.white_queen_rook_moved && !board.white_king_moved)
        {
            board.zobrist_position ^= TT_castle_rights[white_queen_side_castle_right];
        }

        board.white_queen_rook_moved = true;
    }

    if (board.board[board_start + 7] != w_rook)
    {
        if (!board.white_king_rook_moved && !board.white_king_moved)
        {
            board.zobrist_position ^= TT_castle_rights[white_king_side_castle_right];
        }

        board.white_king_rook_moved = true;
    }

    if (board.board[board_end] != b_rook)
    {
        if (!board.black_king_rook_moved && !board.black_king_moved)
        {
            board.zobrist_position ^= TT_castle_rights[black_king_side_castle_right];
        }

        board.black_king_rook_moved = true;
    }

    if (board.board[board_end - 7] != b_rook)
    {
        if (!board.black_queen_rook_moved && !board.black_king_moved)
        {
            board.zobrist_position ^= TT_castle_rights[black_queen_side_castle_right];
        }

        board.black_queen_rook_moved = true;
    }

    board.zobrist_position ^= TT_white;

    return history;
}

inline void undo_move(Board& board, const Move& move, const History& history)
{
    board.en_passant_square = history.en_passant_square;
    board.white_king_rook_moved = history.white_king_rook_moved;
    board.white_queen_rook_moved = history.white_queen_rook_moved;
    board.black_king_rook_moved = history.black_king_rook_moved;
    board.black_queen_rook_moved = history.black_queen_rook_moved;
    board.white_king_position = history.white_king_position;
    board.black_king_position = history.black_king_position;
    board.white_king_moved = history.white_king_moved;
    board.black_king_moved = history.black_king_moved;
    board.zobrist_position = history.zobrist_position;

    int colour = (move.piece > 0) ? white : black;

    board.board[move.end] = move.captured_piece;
    board.board[move.start] = move.piece;

    board.bitboards.bitboards[move.captured_piece + bitboard_offset] |= (1ULL << board_144_to_64[move.end]);

    board.current_material += piece_values[std::abs(move.captured_piece) + piece_value_offset];

    if (move.move_type == capture_promotion_move || move.move_type == promotion_move)
    {
        board.bitboards.bitboards[move.promotion_piece + bitboard_offset] ^= (1ULL << board_144_to_64[move.end]);

        board.current_material += piece_values[std::abs(move.piece) + piece_value_offset];
        board.current_material -= piece_values[std::abs(move.promotion_piece) + piece_value_offset];
    }
    else
    {
        board.bitboards.bitboards[move.piece + bitboard_offset] ^= (1ULL << board_144_to_64[move.end]);
    }

    board.bitboards.bitboards[move.piece + bitboard_offset] |= (1ULL << board_144_to_64[move.start]);

    if (move.move_type == king_side_castle_move)
    {
        if (colour == white)
        {
            board.board[board_start + 7] = w_rook;
            board.board[board_start + 7 - 2] = empty;

            board.bitboards.bitboards[w_rook + bitboard_offset] |= (1ULL << board_144_to_64[board_start + 7]);
            board.bitboards.bitboards[w_rook + bitboard_offset] ^= (1ULL << board_144_to_64[board_start + 7 - 2]);
        }
        else
        {
            board.board[board_end] = b_rook;
            board.board[board_end - 2] = empty;

            board.bitboards.bitboards[b_rook + bitboard_offset] |= (1ULL << board_144_to_64[board_end]);
            board.bitboards.bitboards[b_rook + bitboard_offset] ^= (1ULL << board_144_to_64[board_end - 2]);
        }
    }
    else if (move.move_type == queen_side_castle_move)
    {
        if (colour == white)
        {
            board.board[board_start] = w_rook;
            board.board[board_start + 3] = empty;

            board.bitboards.bitboards[w_rook + bitboard_offset] |= (1ULL << board_144_to_64[board_start]);
            board.bitboards.bitboards[w_rook + bitboard_offset] ^= (1ULL << board_144_to_64[board_start + 3]);
        }
        else
        {
            board.board[board_end - 7] = b_rook;
            board.board[board_end - 7 + 3] = empty;

            board.bitboards.bitboards[b_rook + bitboard_offset] |= (1ULL << board_144_to_64[board_end - 7]);
            board.bitboards.bitboards[b_rook + bitboard_offset] ^= (1ULL << board_144_to_64[board_end - 7 + 3]);
        }
    }
    else if (move.move_type == en_passant_move)
    {
        board.board[move.end + (rows * -colour)] = pawn * -colour;
        board.bitboards.bitboards[(pawn * -colour) + bitboard_offset] |= (1ULL << board_144_to_64[move.end + (rows * -colour)]);

        board.current_material += piece_values[pawn + piece_value_offset];
    }
}

inline NullHistory make_null_move(Board& board)
{
    NullHistory null_history{};
    null_history.en_passant_square = board.en_passant_square;
    null_history.zobrist_position = board.zobrist_position;

    if (board.en_passant_square != no_en_passant)
    {
        int en_passant_column{ board.en_passant_square % columns };
        board.zobrist_position ^= TT_column[en_passant_column];
    }

    board.zobrist_position ^= TT_white;
    board.en_passant_square = no_en_passant;
    return null_history;
}

inline void undo_null_move(Board& board, NullHistory& null_history)
{
    board.en_passant_square = null_history.en_passant_square;
    board.zobrist_position = null_history.zobrist_position;
}

#endif