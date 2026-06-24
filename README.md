# Raspberry
Raspberry is an open-source chess engine written purely in C++. It uses a 12x12 mailbox and bitboard hybrid approach, balancing readability and performance.

## Move Generation
For move generation, Raspberry uses a 12x12 mailbox to represent the board and do piece offset math. It first generates pseudo-legal moves, then filters out the illegal moves based off if the previous side to move's king is still in check after it makes a move.

## Search
Raspberry uses the negamax algorithm with alpha-beta pruning, along with several optimizations, including:
- a transposition table
- iterative deepening
- null move pruning
- move ordering

## Evaluation
Evaluation is where the bitboards come in. When Raspberry makes or undoes a move, it also updates bitboards for each piece. This makes it extremely quick and easy to evaluate things such as pawn structure using bitwise operators. For pawn structure, Raspberry evaluates:
- passed pawns
- stacked pawns (doubled, tripled, etc.)

It also uses middlegame and endgame piece-square tables, which the evaluation interpolates between based on the material on the board, and standard material evaluation.