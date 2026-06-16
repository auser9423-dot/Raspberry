#ifndef GENERATE
#define GENERATE

#include "board.hpp"
#include "pieces.hpp"
#include "colour.hpp"
#include "move.hpp"
#include <array>
#include <cmath>

// Constants
static constexpr std::array<int, 8> knight_offsets{ -25, -23, -14, -10, 10, 14, 23, 25 };
static constexpr std::array<int, 8> king_offsets{ -13, -12, -11, -1, 1, 11, 12, 13 };
static constexpr std::array<int, 4> diagonal_offsets{ -13, -11, 11, 13 };
static constexpr std::array<int, 4> straight_offsets{ -12, -1, 1, 12 };
static constexpr int white_king_start{ board_start + 4 };
static constexpr int black_king_start{ board_end - 3 };

// Pawn movement
static constexpr int single_push{ 12 };
static constexpr int double_push{ single_push * 2 };
static constexpr int left_capture{ 13 };
static constexpr int right_capture{ 11 };
static constexpr int white_promotion_row{ board_end / rows };
static constexpr int black_promotion_row{ board_start / rows };
static constexpr std::array<int, 4> promotion_pieces{ knight, bishop, rook, queen };
static constexpr int white_pawn_start_row{ 3 };
static constexpr int black_pawn_start_row{ 8 };

inline bool is_square_attacked(const Board& board, int square, int attacking_colour)
{
    int left_capture_end{ square - (left_capture * attacking_colour) };
    int left_capture_destination{ board.board[left_capture_end] };
    if (left_capture_destination == pawn * attacking_colour)
    {
        return true;
    }

    int right_capture_end{ square - (right_capture * attacking_colour) };
    int right_capture_destination{ board.board[right_capture_end] };
    if (right_capture_destination == pawn * attacking_colour)
    {
        return true;
    }

    for (size_t i{}; i < knight_offsets.size(); i++)
    {
        int end{ square + knight_offsets[i] };
        int destination{ board.board[end] };
        if (destination == knight * attacking_colour)
        {
            return true;
        }
    }

    for (size_t i{}; i < king_offsets.size(); i++)
    {
        int end{ square + king_offsets[i] };
        int destination{ board.board[end] };
        if (destination == king * attacking_colour)
        {
            return true;
        }
    }

    for (size_t i{}; i < diagonal_offsets.size(); i++)
    {
        int end{ square + diagonal_offsets[i] };
        int destination{ board.board[end] };
        while (destination != out_of_bounds)
        {
            if (destination != empty)
            {
                int piece_type{ std::abs(destination) };
                if ((piece_type == bishop || piece_type == queen) && destination * attacking_colour > 0)
                {
                    return true;
                }
                break;
            }

            end += diagonal_offsets[i];
            destination = board.board[end];
        }
    }

    for (size_t i{}; i < straight_offsets.size(); i++)
    {
        int end{ square + straight_offsets[i] };
        int destination{ board.board[end] };
        while (destination != out_of_bounds)
        {
            if (destination != empty)
            {
                int piece_type{ std::abs(destination) };
                if ((piece_type == rook || piece_type == queen) && destination * attacking_colour > 0)
                {
                    return true;
                }
                break;
            }

            end += straight_offsets[i];
            destination = board.board[end];
        }
    }

    return false;
}

inline Moves generate_pseudo_moves(const Board& board, int colour)
{
    Moves pseudo_moves{};

    for (int start{ board_start }; start <= board_end; start++)
    {
        if (board.board[start] == out_of_bounds || board.board[start] * colour <= 0)
        {
            continue;
        }

        int piece{ board.board[start] };
        int piece_type{ std::abs(piece) };
        if (piece_type == pawn)
        {
            int promotion_row = (colour == white) ? white_promotion_row : black_promotion_row;

            // Single push
            int single_push_end{ start + (single_push * colour) };
            int single_push_destination{ board.board[single_push_end] };
            if (single_push_destination == empty)
            {
                // Single push promotion
                if (single_push_end / rows == promotion_row)
                {
                    for (size_t i{}; i < promotion_pieces.size(); i++)
                    {
                        pseudo_moves.moves[pseudo_moves.move_count] = {start, single_push_end, promotion_pieces[i] * colour, promotion_move, piece, single_push_destination, false};
                        pseudo_moves.move_count++;
                    }
                }
                else
                {
                    pseudo_moves.moves[pseudo_moves.move_count] = {start, single_push_end, none, none, piece, single_push_destination, false};
                    pseudo_moves.move_count++;

                    // Double push
                    int pawn_start_row = (colour == white) ? white_pawn_start_row : black_pawn_start_row;
                    if (start / rows == pawn_start_row)
                    {
                        int double_push_end{ start + (double_push * colour) };
                        int double_push_destination{ board.board[double_push_end] };
                        if (double_push_destination == empty)
                        {
                            pseudo_moves.moves[pseudo_moves.move_count] = {start, double_push_end, none, double_push_move, piece, double_push_destination, false};
                            pseudo_moves.move_count++;
                        }
                    }
                }
            }

            // Left capture
            int left_capture_end{ start + (left_capture * colour) };
            int left_capture_destination{ board.board[left_capture_end] };
            if (left_capture_destination != out_of_bounds && left_capture_destination * colour < 0)
            {
                // Left capture promotion
                if (left_capture_end / rows == promotion_row)
                {
                    for (size_t i{}; i < promotion_pieces.size(); i++)
                    {
                        pseudo_moves.moves[pseudo_moves.move_count] = {start, left_capture_end, promotion_pieces[i] * colour, capture_promotion_move, piece, left_capture_destination, false};
                        pseudo_moves.move_count++;
                    }
                }
                else
                {
                    pseudo_moves.moves[pseudo_moves.move_count] = {start, left_capture_end, none, none, piece, left_capture_destination, false};
                    pseudo_moves.move_count++;
                }
            }
            else if (left_capture_end == board.en_passant_square)
            {
                pseudo_moves.moves[pseudo_moves.move_count] = {start, left_capture_end, none, en_passant_move, piece, left_capture_destination, false};
                pseudo_moves.move_count++;
            }

            // Right capture
            int right_capture_end{ start + (right_capture * colour) };
            int right_capture_destination{ board.board[right_capture_end] };
            if (right_capture_destination != out_of_bounds && right_capture_destination * colour < 0)
            {
                // Right capture promotion
                if (right_capture_end / rows == promotion_row)
                {
                    for (size_t i{}; i < promotion_pieces.size(); i++)
                    {
                        pseudo_moves.moves[pseudo_moves.move_count] = {start, right_capture_end, promotion_pieces[i] * colour, capture_promotion_move, piece, right_capture_destination, false};
                        pseudo_moves.move_count++;
                    }
                }
                else
                {
                    pseudo_moves.moves[pseudo_moves.move_count] = {start, right_capture_end, none, none, piece, right_capture_destination, false};
                    pseudo_moves.move_count++;
                }
            }
            else if (right_capture_end == board.en_passant_square)
            {
                pseudo_moves.moves[pseudo_moves.move_count] = {start, right_capture_end, none, en_passant_move, piece, right_capture_destination, false};
                pseudo_moves.move_count++;
            }
        }
        else if (piece_type == knight)
        {
            for (size_t i{}; i < knight_offsets.size(); i++)
            {
                int end{ start + knight_offsets[i] };
                int destination{ board.board[end] };
                if (destination != out_of_bounds && destination * colour <= 0)
                {
                    pseudo_moves.moves[pseudo_moves.move_count] = {start, end, none, none, piece, destination, false};
                    pseudo_moves.move_count++;
                }
            }
        }
        else if (piece_type == king)
        {
            for (size_t i{}; i < king_offsets.size(); i++)
            {
                int end{ start + king_offsets[i] };
                int destination{ board.board[end] };
                if (destination != out_of_bounds && destination * colour <= 0)
                {
                    pseudo_moves.moves[pseudo_moves.move_count] = {start, end, none, none, piece, destination, false};
                    pseudo_moves.move_count++;
                }
            }

            if (colour == white)
            {
                if (!board.white_king_moved && board.white_king_position == white_king_start && !is_square_attacked(board, white_king_start, black))
                {
                    // Kingside castling
                    if
                    (
                        !is_square_attacked(board, white_king_start + 1, black) && board.board[white_king_start + 1] == empty
                        && !is_square_attacked(board, white_king_start + 2, black) && board.board[white_king_start + 2] == empty
                        && !board.white_king_rook_moved
                    )
                    {
                        pseudo_moves.moves[pseudo_moves.move_count] = {start, white_king_start + 2, none, king_side_castle_move, piece, board.board[white_king_start + 2], false};
                        pseudo_moves.move_count++;
                    }

                    // Queenside castling
                    if
                    (
                        !is_square_attacked(board, white_king_start - 1, black) && board.board[white_king_start - 1] == empty
                        && !is_square_attacked(board, white_king_start - 2, black) && board.board[white_king_start - 2] == empty
                        && board.board[white_king_start - 3] == empty && !board.white_queen_rook_moved
                    )
                    {
                        pseudo_moves.moves[pseudo_moves.move_count] = {start, white_king_start - 2, none, queen_side_castle_move, piece, board.board[white_king_start - 2], false};
                        pseudo_moves.move_count++;
                    }
                }
            }
            else
            {
                if (!board.black_king_moved && board.black_king_position == black_king_start && !is_square_attacked(board, black_king_start, white))
                {
                    // Kingside castling
                    if
                    (
                        !is_square_attacked(board, black_king_start + 1, white) && board.board[black_king_start + 1] == empty
                        && !is_square_attacked(board, black_king_start + 2, white) && board.board[black_king_start + 2] == empty
                        && !board.black_king_rook_moved
                    )
                    {
                        pseudo_moves.moves[pseudo_moves.move_count] = {start, black_king_start + 2, none, king_side_castle_move, piece, board.board[black_king_start + 2], false};
                        pseudo_moves.move_count++;
                    }

                    // Queenside castling
                    if
                    (
                        !is_square_attacked(board, black_king_start - 1, white) && board.board[black_king_start - 1] == empty
                        && !is_square_attacked(board, black_king_start - 2, white) && board.board[black_king_start - 2] == empty
                        && board.board[black_king_start - 3] == empty && !board.black_queen_rook_moved
                    )
                    {
                        pseudo_moves.moves[pseudo_moves.move_count] = {start, black_king_start - 2, none, queen_side_castle_move, piece, board.board[black_king_start - 2], false};
                        pseudo_moves.move_count++;
                    }
                }
            }

        }
        else if (piece_type == bishop || piece_type == queen)
        {
            for (size_t i{}; i < diagonal_offsets.size(); i++)
            {
                int end{ start + diagonal_offsets[i] };
                int destination{ board.board[end] };
                while (destination != out_of_bounds && destination * colour <= 0)
                {
                    pseudo_moves.moves[pseudo_moves.move_count] = {start, end, none, none, piece, destination, false};
                    pseudo_moves.move_count++;

                    if (destination != empty)
                    {
                        break;
                    }

                    end += diagonal_offsets[i];
                    destination = board.board[end];
                }
            }
        }

        if (piece_type == rook || piece_type == queen)
        {
            for (size_t i{}; i < straight_offsets.size(); i++)
            {
                int end{ start + straight_offsets[i] };
                int destination{ board.board[end] };
                while (destination != out_of_bounds && destination * colour <= 0)
                {
                    pseudo_moves.moves[pseudo_moves.move_count] = {start, end, none, none, piece, destination, false};
                    pseudo_moves.move_count++;

                    if (destination != empty)
                    {
                        break;
                    }

                    end += straight_offsets[i];
                    destination = board.board[end];
                }
            }
        }
    }

    return pseudo_moves;
}

inline Moves generate_legal_moves(Board& board, int colour)
{
    Moves pseudo_moves{ generate_pseudo_moves(board, colour) };
    for (size_t i{}; i < pseudo_moves.move_count; i++)
    {
        Move& move{ pseudo_moves.moves[i] };
        History history{ make_move(board, move) };
        int king_position = (colour == white) ? board.white_king_position : board.black_king_position;

        if (!is_square_attacked(board, king_position, -colour))
        {
            pseudo_moves.moves[i].is_legal = true;
        }

        undo_move(board, move, history);
    }

    return pseudo_moves;
}

#endif