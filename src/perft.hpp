#ifndef PERFT
#define PERFT

#include "move.hpp"
#include "generate.hpp"
#include "board.hpp"

unsigned long long perft(Board& board, int colour, int depth)
{
    unsigned long long nodes{};

    if (depth == 0)
    {
        return 1ULL;
    }

    Moves legal_moves{ generate_legal_moves(board, colour) };
    for (int i{}; i < legal_moves.move_count; i++)
    {
        Move& move{ legal_moves.moves[i] };
        if (move.is_legal)
        {
            History history{ make_move(board, move) };
            nodes += perft(board, -colour, depth - 1);
            undo_move(board, move, history);
        }
    }
    return nodes;
}

#endif