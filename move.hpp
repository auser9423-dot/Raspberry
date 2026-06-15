#ifndef MOVE
#define MOVE

#include "board.hpp"
#include "bitboard.hpp"
#include "colour.hpp"
#include "pieces.hpp"
#include <array>

// Constants 
static constexpr int max_moves{ 218 };

// Move types
static constexpr int en_passant_move{};
static constexpr int double_push_move{ 1 };
static constexpr int promotion_move{ 2 };
static constexpr int capture_promotion_move{ 3 };
static constexpr int king_side_castle_move{ 4 };
static constexpr int queen_side_castle_move{ 5 };

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

History make_move(Board& board, const Move& move)
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

    int colour = (move.piece > 0) ? white : black;

    board.board[move.start] = empty;
    if (move.move_type == promotion_move || move.move_type == capture_promotion_move)
    {
        board.board[move.end] = move.promotion_piece;
    }
    else
    {
        board.board[move.end] = move.piece;
        if (move.move_type == king_side_castle_move)
        {
            if (colour == white)
            {
                board.white_king_moved = true;
                board.white_king_rook_moved = true;

                board.board[board_start + 7] = empty;
                board.board[board_start + 7 - 2] = w_rook;

                board.white_king_position = move.end;
            }
            else
            {
                board.black_king_moved = true;
                board.black_king_rook_moved = true;

                board.board[board_end] = empty;
                board.board[board_end - 2] = b_rook;

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

                board.white_king_position = move.end;
            }
            else
            {
                board.black_king_moved = true;
                board.black_queen_rook_moved = true;

                board.board[board_end - 7] = empty;
                board.board[board_end - 7 + 3] = b_rook;

                board.black_king_position = move.end;
            }
        }
        else if (move.move_type == en_passant_move)
        {
            board.board[move.end + (rows * -colour)] = empty;
        }
        else if (move.move_type == double_push_move)
        {
            board.en_passant_square = move.end + (rows * -colour);
        }
    }

    if (move.piece == w_king)
    {
        board.white_king_position = move.end;
        board.white_king_moved = true;
    }
    else if (move.piece == b_king)
    {
        board.black_king_position = move.end;
        board.black_king_moved = true;
    }
    else if (move.piece == w_rook)
    {
        if (move.start % columns == 2)
        {
            board.white_queen_rook_moved = true;
        }
        else if (move.start % columns == 9)
        {
            board.white_king_rook_moved = true;
        }
    }
    else if (move.piece == b_rook)
    {
        if (move.start % columns == 2)
        {
            board.black_queen_rook_moved = true;
        }
        else if (move.start % columns == 9)
        {
            board.black_king_rook_moved = true;
        }
    }

    return history;
}

void undo_move(Board& board, const Move& move, const History& history)
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

    if (move.move_type == king_side_castle_move)
    {
        if (colour == white)
        {
            board.board[board_start + 7] = w_rook;
            board.board[board_start + 7 - 2] = empty;
        }
        else
        {
            board.board[board_end] = b_rook;
            board.board[board_end - 2] = empty;
        }
    }
    else if (move.move_type == queen_side_castle_move)
    {
        if (colour == white)
        {
            board.board[board_start] = w_rook;
            board.board[board_start + 3] = empty;
        }
        else
        {
            board.board[board_end - 7] = b_rook;
            board.board[board_end - 7 + 3] = empty;
        }
    }
    else if (move.move_type == en_passant_move)
    {
        board.board[move.end + (rows * -colour)] = pawn * -colour;
    }
}

#endif