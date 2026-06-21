#ifndef BITBOARD
#define BITBOARD

#include <cstdint>
#include <array>

using Bitboard = uint64_t;

// Constants
inline static constexpr int bitboard_offset{ 6 };

struct Bitboards
{
    std::array<Bitboard, 13> bitboards{};
};

#endif