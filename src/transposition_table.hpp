#ifndef TRANSPOSITION_TABLE
#define TRANSPOSITION_TABLE

#include "move.hpp"
#include <array>
#include <cstdint>
#include <vector>

inline static constexpr int TT_size{ 268435456 };
inline static constexpr int exact_flag{ 1 };
inline static constexpr int upper_bound_flag{ 2 };
inline static constexpr int lower_bound_flag{ 3 };

struct TTEntry
{
    uint64_t key{};
    Move best_move{};
    int depth{};
    int score{};
    int flag{};
};

std::vector<TTEntry> TT{ std::vector<TTEntry>(TT_size) };

#endif