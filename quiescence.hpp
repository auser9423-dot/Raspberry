#ifndef QUIESCENCE
#define QUIESCENCE

#include "board.hpp"
#include "evaluate.hpp"
#include "generate.hpp"
#include "move.hpp"

int quiescence(Board& board, int alpha, int beta, int colour)
{
    int static_eval{ evaluate(board, colour) };
    int best_score{ static_eval };
    if (best_score >= beta)
    {
        return best_score;
    }

    if (best_score > alpha)
    {
        alpha = best_score;
    }

    Moves legal_moves{ generate_legal_moves(board, colour) };
    order_moves(legal_moves);
    
    for (int i{}; i < legal_moves.move_count; i++)
    {
        Move move{ legal_moves.moves[i] };
        if (move.is_legal && (move.captured_piece != empty || move.move_type == en_passant_move))
        {
            History history{ make_move(board, move) };
            int score{ -quiescence(board, -beta, -alpha, -colour) };
            undo_move(board, move, history);

            if (score >= beta)
            {
                return score;
            }

            if (score > best_score)
            {
                best_score = score;
            }

            if (score > alpha)
            {
                alpha = score;
            }
        }
    }
    return best_score;
}

#endif