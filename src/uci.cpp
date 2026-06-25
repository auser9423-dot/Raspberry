#include "pieces.hpp"
#include "board.hpp"
#include "generate.hpp"
#include "move.hpp"
#include "negamax.hpp"
#include <iostream>
#include <string>
#include <sstream>

int promotion_piece_to_value(char promotion_piece)
{
    switch (promotion_piece)
    {
        case 'n':
        case 'N':
            return knight;
        case 'b':
        case 'B':
            return bishop;
        case 'r':
        case 'R':
            return rook;
        case 'q':
        case 'Q':
            return queen;
        default:
            return none;
    }
}

int main()
{
    Board board{};
    int colour{ 1 };

    std::string uci_input;
    while (true)
    {
        std::getline(std::cin, uci_input);
        if (uci_input == "uci")
        {
            std::cout << "id name Raspberry-Debut\n";
            std::cout << "id author auser9423-dot\n";
            std::cout << "uciok" << std::endl;            
        }
        else if (uci_input == "isready")
        {
            std::cout << "readyok" << std::endl;
        }
        else if (uci_input == "ucinewgame")
        {
            board = Board();
            board.setup_default_start_position();
            colour = white;
        }
        else if (uci_input == "quit")
        {
            break;
        }
        else
        {
            std::stringstream ss(uci_input);
            std::string word;

            bool is_moves{ false };

            while (ss >> word)
            {
                if (word == "position")
                {
                    ss >> word;
                    if (word == "startpos")
                    {
                        board = Board();
                        board.setup_default_start_position();
                        colour = white;

                        ss >> word; // This is the "moves" word
                        is_moves = true;
                    }
                }
                else if (word == "go")
                {
                    int depth{ 20 }; // 11
                    int alpha{ negative_infinity };
                    int beta{ -alpha };

                    ss >> word;
                    if (word == "depth")
                    {
                        ss >> depth;
                    }

                    Move best_move{ search(board, alpha, beta, colour, depth) };
                    std::string best_move_converted{ index_to_square[best_move.start] + index_to_square[best_move.end] };
                    if (best_move.promotion_piece != none)
                    {
                        best_move_converted += promotion_value_to_piece(best_move.promotion_piece);
                    }
            
                    std::cout << "bestmove " << best_move_converted << std::endl;
                }

                if (is_moves)
                {
                    std::string move;
                    while (ss >> move)
                    {
                        int start{ square_to_index({move[0], move[1]}) };
                        int end{ square_to_index({move[2], move[3]}) };

                        int promotion_piece = (move.size() == 5) ? promotion_piece_to_value(move[4]) * colour : none;

                        Moves legal_moves{ generate_legal_moves(board, colour) };
                        for (int i{}; i < legal_moves.move_count; i++)
                        {
                            Move found_move{ legal_moves.moves[i] };
                            if (found_move.start == start && found_move.end == end && found_move.promotion_piece == promotion_piece)
                            {
                                make_move(board, found_move);
                                break;
                            }
                        }

                        colour = -colour;
                    }

                    is_moves = false;
                }
            }
        }
    }

    return 0;
}

// g++ -O3 -std=c++20 ./src/uci.cpp -o raspberry