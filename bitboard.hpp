#ifndef BITBOARD
#define BITBOARD

#include <cstdint>

using Bitboard = uint64_t;

struct Bitboards
{
    Bitboard w_pawn{};
    Bitboard w_knight{};
    Bitboard w_bishop{};
    Bitboard w_rook{};
    Bitboard w_queen{};
    Bitboard w_king{};

    Bitboard b_pawn{};
    Bitboard b_knight{};
    Bitboard b_bishop{};
    Bitboard b_rook{};
    Bitboard b_queen{};
    Bitboard b_king{};
};

#endif