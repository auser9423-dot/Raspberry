#include "board.hpp"
#include <iostream>
#include <string>
#include <sstream>

int main()
{
    Board board{};

    std::string uci_input;
    while (true)
    {
        std::getline(std::cin, uci_input);
        if (uci_input == "uci")
        {
            std::cout << "uciok" << std::endl;
        }
        else if (uci_input == "isready")
        {
            std::cout << "readyok" << std::endl;
        }
        else if (uci_input == "ucinewgame")
        {
            board = Board();
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
                        ss >> word; // This is the "moves" word
                        is_moves = true;
                    }
                }

                if (is_moves)
                {
                    
                }
            }
        }
    }
}