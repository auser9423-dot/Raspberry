#ifndef QUIESCENCE
#define QUIESCENCE

#include "board.hpp"
#include "evaluate.hpp"
#include "generate.hpp"
#include "move.hpp"
#include "pieces.hpp"

int quiescence(Board& board, int alpha, int beta, int colour, uint64_t& nodes)
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

    TTEntry& TT_entry{ TT[board.zobrist_position % TT_size] };
    Move hash_move{};

    if (TT_entry.key == board.zobrist_position)
    {
        if (TT_entry.best_move.piece != none)
        {
            hash_move = TT_entry.best_move;
        }
    }

    Moves legal_moves{ generate_legal_moves(board, colour) };
    order_moves(board, legal_moves, hash_move, colour, 0);
    
    for (int i{}; i < legal_moves.move_count; i++)
    {
        Move move{ legal_moves.moves[i] };
        if (move.is_legal && (move.captured_piece != empty || move.move_type == en_passant_move || std::abs(move.promotion_piece) != none))
        {
            History history{ make_move(board, move) };
            nodes++;
            
            int score{ -quiescence(board, -beta, -alpha, -colour, nodes) };
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