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

    auto start{ std::chrono::steady_clock::now() };
    unsigned long long nodes{ perft(board, colour, depth) };
    auto end{ std::chrono::steady_clock::now() };

    std::chrono::duration<double> seconds{ end - start };
    double nps{ nodes / seconds.count() };

    std::cout << "Depth: " << depth << '\n' << "Nodes: " << nodes << '\n' << "NPS: " << static_cast<unsigned long long>(nps);

    return 0;
}

// g++ -O3 -std=c++20 perft.cpp -o perft