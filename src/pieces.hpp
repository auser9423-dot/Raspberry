#ifndef PIECES
#define PIECES

#include <array>

// Constants
inline static constexpr int w_pawn{ 1 };
inline static constexpr int w_knight{ 2 };
inline static constexpr int w_bishop{ 3 };
inline static constexpr int w_rook{ 4 };
inline static constexpr int w_queen{ 5 };
inline static constexpr int w_king{ 6 };

inline static constexpr int b_pawn{ -1 };
inline static constexpr int b_knight{ -2 };
inline static constexpr int b_bishop{ -3 };
inline static constexpr int b_rook{ -4 };
inline static constexpr int b_queen{ -5 };
inline static constexpr int b_king{ -6 };

// "Neutral" pieces
inline static constexpr int pawn{ 1 };
inline static constexpr int knight{ 2 };
inline static constexpr int bishop{ 3 };
inline static constexpr int rook{ 4 };
inline static constexpr int queen{ 5 };
inline static constexpr int king{ 6 };

inline static constexpr int none{};

inline static constexpr std::array<int, 13> piece_values
{
    // b_king, b_queen, b_rook, b_bishop, b_knight, b_pawn, garbage
       -10000, -1000,   -525,   -350,     -325,     -100,   0,
    // w_pawn, w_knight, w_bishop, w_rook, w_queen, w_king
       100,    325,      350,      525,    1000,    10000
};

inline static constexpr int piece_value_offset{ 6 };

#endif