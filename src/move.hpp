#ifndef MOVE
#define MOVE

#include "board.hpp"
#include "bitboard.hpp"
#include "colour.hpp"
#include "pieces.hpp"
#include <array>
#include <cmath>

// Constants 
inline static constexpr int max_moves{ 218 };

// Move types
inline static constexpr int en_passant_move{ 6 };
inline static constexpr int double_push_move{ 1 };
inline static constexpr int promotion_move{ 2 };
inline static constexpr int capture_promotion_move{ 3 };
inline static constexpr int king_side_castle_move{ 4 };
inline static constexpr int queen_side_castle_move{ 5 };

struct Move
{
    int start{};
    int end{};
    int promotion_piece{};
    int move_type{};
    int piece{};
    int captured_piece{};
    bool is_legal{};
};

struct Moves
{
    int move_count{};
    std::array<Move, max_moves> moves;
};

struct History
{
    int en_passant_square{};
    bool white_king_rook_moved{};
    bool white_queen_rook_moved{};
    bool black_king_rook_moved{};
    bool black_queen_rook_moved{};
    int white_king_position{};
    int black_king_position{};
    bool white_king_moved{};
    bool black_king_moved{};
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

    board.en_passant_square = no_en_passant;

    int colour = (move.piece > 0) ? white : black;

    board.board[move.start] = empty;
    board.bitboards.bitboards[move.piece + bitboard_offset] ^= (1ULL << board_144_to_64[move.start]);
    board.bitboards.bitboards[move.captured_piece + bitboard_offset] ^= (1ULL << board_144_to_64[move.end]);

    board.current_material -= piece_values[std::abs(move.captured_piece) + piece_value_offset];

    if (move.move_type == promotion_move || move.move_type == capture_promotion_move)
    {
        board.board[move.end] = move.promotion_piece;
        board.bitboards.bitboards[move.promotion_piece + bitboard_offset] |= (1ULL << board_144_to_64[move.end]);

        board.current_material -= piece_values[std::abs(move.piece) + piece_value_offset];
        board.current_material += piece_values[std::abs(move.promotion_piece) + piece_value_offset];
    }
    else
    {
        board.board[move.end] = move.piece;
        board.bitboards.bitboards[move.piece + bitboard_offset] |= (1ULL << board_144_to_64[move.end]);

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

                board.black_king_position = move.end;
            }
        }
        else if (move.move_type == en_passant_move)
        {
            board.board[move.end + (rows * -colour)] = empty;
            board.bitboards.bitboards[(pawn * -colour) + bitboard_offset] ^= (1ULL << board_144_to_64[move.end + (rows * -colour)]);

            board.current_material -= piece_values[pawn + piece_value_offset];
        }
        else if (move.move_type == double_push_move)
        {
            board.en_passant_square = move.end + (rows * -colour);
        }
    }

    if (board.white_king_position != board_start + 4)
    {
        board.white_king_moved = true;
    }

    if (board.board[board_start] != w_rook)
    {
        board.white_queen_rook_moved = true;
    }

    if (board.board[board_start + 7] != w_rook)
    {
        board.white_king_rook_moved = true;
    }

    if (board.board[board_end] != b_rook)
    {
        board.black_king_rook_moved = true;
    }

    if (board.board[board_end - 7] != b_rook)
    {
        board.black_queen_rook_moved = true;
    }

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

inline int make_null_move(Board& board)
{
    int en_passant_square{ board.en_passant_square };

    board.en_passant_square = no_en_passant;
    return en_passant_square;
}

inline void undo_null_move(Board& board, int en_passant_square)
{
    board.en_passant_square = en_passant_square;
}

#endif