#include <iostream>
#include <chrono>

#include "perft.hpp"
#include "board.hpp"

int main()
{
    Board board{};
    board.setup_default_start_position();

    int depth{ 7 };
    int colour{ 1 };

    unsigned long long nodes{ perft(board, colour, depth) };
    std::cout << "Depth: " << depth << '\n' << "Nodes: " << nodes;

    return 0;
}

// g++ -O3 -std=c++20 perft.cpp -o perft