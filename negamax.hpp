#ifndef NEGAMAX
#define NEGAMAX

#include "move.hpp"

inline int negamax(Board& board, int alpha, int beta, int colour, int depth)
{
    if (depth == 0)
    {
        return quiescence(board, alpha, beta, colour);
    }

    int best_score{ -1000000000 };

    Moves legal_moves{ generate_legal_moves(board, colour) };
    for (int i{}; i < legal_moves.move_count; i++)
    {
        Move move{ legal_moves.moves[i] };
        if (move.is_legal)
        {
            History history{ make_move(board, move) };
            int score{ -negamax(board, -beta, -alpha, -colour, depth - 1) };
            undo_move(board, move, history);

            if (score > best_score)
            {
                best_score = score;
                if (score > alpha)
                {
                    alpha = score;
                }
            }

            if (score >= beta)
            {
                return best_score;
            }
        }
    }
    return best_score;
}

#endif