#ifndef NEGAMAX
#define NEGAMAX

#include "move.hpp"
#include "quiescence.hpp"
#include "evaluate.hpp"

inline static constexpr int negative_infinity{ -1000000000 };
inline static constexpr int null_move_reduction{ 2 };

inline int negamax(Board& board, int alpha, int beta, int colour, int depth, bool is_null_move)
{
    if (depth == 0)
    {
        return quiescence(board, alpha, beta, colour);
    }

    int best_score{ negative_infinity };

    // Null move pruning
    int king_position = (colour == white) ? board.white_king_position : board.black_king_position;
    if (board.current_material > 0 && depth >= 4 && !is_square_attacked(board, king_position, -colour) && !is_null_move)
    {
        int en_passant_square{ make_null_move(board) };
        int null_move_score{ -negamax(board, -beta, -(beta - 1), -colour, depth - 1 - null_move_reduction, true) };
        undo_null_move(board, en_passant_square);

        if (null_move_score >= beta)
        {
            return null_move_score;
        }
    }

    Moves legal_moves{ generate_legal_moves(board, colour) };
    order_moves(legal_moves);

    int moves_played{};

    for (int i{}; i < legal_moves.move_count; i++)
    {
        Move move{ legal_moves.moves[i] };
        if (move.is_legal)
        {
            History history{ make_move(board, move) };
            int score{ -negamax(board, -beta, -alpha, -colour, depth - 1, false) };
            undo_move(board, move, history);

            if (score > best_score)
            {
                best_score = score;
                if (score > alpha)
                {
                    alpha = score;
                }
            }

            if (score >= beta) // Prune because the opponent wouldn't let that position happen
            {
                return best_score;
            }

            moves_played++;
        }
    }

    if (moves_played == 0)
    {
        if (is_square_attacked(board, king_position, -colour))
        {
            return negative_infinity + (100 - depth);
        }

        return 0;
    }

    return best_score;
}

inline Move search(Board& board, int alpha, int beta, int colour, int depth)
{
    Moves legal_moves{ generate_legal_moves(board, colour) };
    Move best_move{};
    int best_score{ negative_infinity };


    order_moves(legal_moves);
    for (int i{}; i < legal_moves.move_count; i++)
    {
        Move move{ legal_moves.moves[i] };
        if (move.is_legal)
        {
            History history{ make_move(board, move) };
            int score{ -negamax(board, -beta, -alpha, -colour, depth - 1, false) };
            undo_move(board, move, history);

            if (score > best_score)
            {
                best_move = move;
                best_score = score;
            }
        }
    }
    return best_move;
}

#endif