#ifndef BITMASKS
#define BITMASKS

#include "board.hpp"
#include <array>
#include <cstdint>

// Passed pawn advantage
inline static constexpr std::array<uint64_t, 64> b_passed_pawn_mask = []()
{
    std::array<uint64_t, 64> temp{};
    for (int i{}; i < 64; i++)
    {
        int i_row{ i / 8 };
        if (i_row != 0 && i_row != 7)
        {
            int i_column{ i % 8 };
            uint64_t bitmask{};

            for (int j{}; j < 64; j++)
            {
                int j_column{ j % 8 };
                int j_row{ j / 8 };
                if (j_row < i_row && (j_column == i_column - 1 || j_column == i_column || j_column == i_column + 1))
                {
                    bitmask |= (1ULL << j);
                }
            }

            temp[i] = bitmask;
        }
    }
    return temp;
}();

inline static constexpr std::array<uint64_t, 64> w_passed_pawn_mask = []()
{
    std::array<uint64_t, 64> temp{};
    for (int i{}; i < 64; i++)
    {
        int i_row{ i / 8 };
        if (i_row != 0 && i_row != 7)
        {
            int i_column{ i % 8 };
            uint64_t bitmask{};

            for (int j{}; j < 64; j++)
            {
                int j_column{ j % 8 };
                int j_row{ j / 8 };
                if (j_row > i_row && (j_column == i_column - 1 || j_column == i_column || j_column == i_column + 1))
                {
                    bitmask |= (1ULL << j);
                }
            }

            temp[i] = bitmask;
        }
    }
    return temp;
}();

// Stacked pawn disavantage
inline static constexpr std::array<uint64_t, 8> stacked_pawns_mask = []()
{
    std::array<uint64_t, 8> temp{};
    for (int i{}; i < 8; i++)
    {
        uint64_t bitmask{};

        int column{ i % 8 };
        for (int j{}; j < 64; j++)
        {
            if (j % 8 == column)
            {
                bitmask |= (1ULL << j);
            }
        }

        temp[i] = bitmask;
    }
    return temp;
}();

#endif