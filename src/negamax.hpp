#ifndef NEGAMAX
#define NEGAMAX

#include "move.hpp"
#include "quiescence.hpp"
#include "evaluate.hpp"
#include "transposition_table.hpp"
#include <iostream>
#include <string>

inline static constexpr int negative_infinity{ -1000000000 };
inline static constexpr int null_move_reduction{ 2 };
inline static constexpr int max_history{ 550000 };

inline int negamax(Board& board, int alpha, int beta, int colour, int depth, int ply, bool is_null_move)
{
    if (board.moves_since_pawn_move >= 100)
    {
        return 0;
    }

    if (!is_null_move)
    {
        int same_position_counter{};
        int limit{ std::max(0, static_cast<int>(board.previous_positions.size()) - board.moves_since_pawn_move - 1) };

        for (int i{ static_cast<int>(board.previous_positions.size()) - 1 }; i >= limit; i -= 2)
        {
            if (board.previous_positions[i] == board.zobrist_position)
            {
                same_position_counter++;
                if (same_position_counter == 3)
                {
                    return 0;
                }
            }
        }
    }

    int original_alpha{ alpha };

    TTEntry& TT_entry{ TT[board.zobrist_position % TT_size] };

    Move hash_move{};

    if (TT_entry.key == board.zobrist_position)
    {
        if (TT_entry.depth >= depth)
        {
            if
            (
                (TT_entry.flag == exact_flag)
                || (TT_entry.flag == lower_bound_flag && TT_entry.score >= beta)
                || (TT_entry.flag == upper_bound_flag && TT_entry.score <= alpha)
            )
            {
                return TT_entry.score;
            }
        }

        if (TT_entry.best_move.piece != none)
        {
            hash_move = TT_entry.best_move;
        }
    }

    if (depth == 0)
    {
        return quiescence(board, alpha, beta, colour);
    }

    int best_score{ negative_infinity };

    // Null move pruning
    int king_position = (colour == white) ? board.white_king_position : board.black_king_position;
    if (board.current_material > 0 && depth >= 4 && !is_square_attacked(board, king_position, -colour) && !is_null_move)
    {
        NullHistory null_history{ make_null_move(board) };
        int null_move_score{ -negamax(board, -beta, -(beta - 1), -colour, depth - 1 - null_move_reduction, ply + 1, true) };
        undo_null_move(board, null_history);

        if (null_move_score >= beta)
        {
            return null_move_score;
        }
    }

    Moves legal_moves{ generate_legal_moves(board, colour) };
    order_moves(board, legal_moves, hash_move, colour, ply);

    int moves_played{};
    Move best_move{};

    for (int i{}; i < legal_moves.move_count; i++)
    {
        Move& move{ legal_moves.moves[i] };
        if (move.is_legal)
        {
            History history{ make_move(board, move) };

            int score{ -negamax(board, -beta, -alpha, -colour, depth - 1, ply + 1, false) };

            undo_move(board, move, history);

            moves_played++;

            if (score > best_score)
            {
                best_score = score;
                best_move = move;

                if (score > alpha)
                {
                    alpha = score;
                }
            }

            if (score >= beta) // Prune because the opponent wouldn't let that position happen
            {
                // Killer moves update
                if (move.captured_piece == none && move.promotion_piece == none && move.move_type != en_passant_move)
                {
                    Move& killer_move_1{ board.killer_table[ply][0] };
                    if (killer_move_1.start != move.start || killer_move_1.end != move.end || killer_move_1.promotion_piece != move.promotion_piece)
                    {
                        Move& killer_move_2{ board.killer_table[ply][1] };
                        killer_move_2 = killer_move_1;
                        killer_move_1 = move;
                    }
                }

                // History moves update
                if (move.captured_piece == none && move.promotion_piece == none && move.move_type != en_passant_move)
                {
                    int bonus{ depth * depth };
                    int clamped_bonus{ std::clamp(bonus, -max_history, max_history) };
                    board.history_table[colour + history_table_offset][move.start][move.end] +=
                    clamped_bonus - board.history_table[colour + history_table_offset][move.start][move.end] * std::abs(clamped_bonus) / max_history;
                }

                break;
            }
        }
    }

    if (moves_played == 0)
    {
        if (is_square_attacked(board, king_position, -colour))
        {
            best_score = negative_infinity + (100 - depth);
        }
        else
        {
            best_score = 0;
        }
    }

    if (depth >= TT_entry.depth)
    {
        TT_entry.best_move = best_move;
        TT_entry.score = best_score;
        TT_entry.depth = depth;
        TT_entry.key = board.zobrist_position;

        if (best_score <= original_alpha)
        {
            TT_entry.flag = upper_bound_flag;
        }
        else if (best_score >= beta)
        {
            TT_entry.flag = lower_bound_flag;
        }
        else
        {
            TT_entry.flag = exact_flag;
        }
    }

    return best_score;
}

inline char promotion_value_to_piece(int promotion_piece)
{
    switch (std::abs(promotion_piece))
    {
        case knight:
            return 'n';
        case bishop:
            return 'b';
        case rook:
            return 'r';
        case queen:
            return 'q';
    }
}

inline Move search(Board& board, int alpha, int beta, int colour, int depth)
{
    Move best_move{};
    Moves legal_moves{ generate_legal_moves(board, colour) };

    for (int current_depth{ 1 }; current_depth <= depth; current_depth++)
    {
        int best_score{ negative_infinity };

        alpha = negative_infinity;
        beta = -negative_infinity;

        TTEntry& TT_entry{ TT[board.zobrist_position % TT_size] };

        Move hash_move{};

        if (TT_entry.key == board.zobrist_position)
        {
            if (TT_entry.best_move.piece != none)
            {
                hash_move = TT_entry.best_move;
            }
        }

        order_moves(board, legal_moves, hash_move, colour, 0);
        for (int i{}; i < legal_moves.move_count; i++)
        {
            Move move{ legal_moves.moves[i] };
            if (move.is_legal)
            {
                History history{ make_move(board, move) };
                int score{ -negamax(board, -beta, -alpha, -colour, current_depth - 1, 0, false) };
                undo_move(board, move, history);

                if (score > best_score)
                {
                    best_move = move;
                    best_score = score;

                    if (score > alpha)
                    {
                        alpha = score;
                    }
                }

                if (score >= beta)
                {
                    break; 
                }
            }
        }

        TT_entry.best_move = best_move;
        TT_entry.score = best_score;
        TT_entry.depth = current_depth;
        TT_entry.key = board.zobrist_position;
        TT_entry.flag = exact_flag;

        std::string best_move_converted{ index_to_square[best_move.start] + index_to_square[best_move.end] };
        if (best_move.promotion_piece != none)
        {
            best_move_converted += promotion_value_to_piece(best_move.promotion_piece);
        }
        std::cout << "info" << " depth " << current_depth << " score cp " << best_score << " pv " << best_move_converted << std::endl;
    }

    return best_move;
}

#endif