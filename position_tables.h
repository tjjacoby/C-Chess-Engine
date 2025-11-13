#ifndef POSITION_TABLES_H
#define POSITION_TABLES_H

// Piece-Square Tables for positional evaluation



// Layout: H1=0, G1=1, F1=2... A1=7, H2=8... A8=63
// Values encourage pawn advancement toward promotion
// White pawns: start rank 2, promote rank 8
// Black pawns: start rank 7, promote rank 8

// Rescaled pawn tables (moderate range, avoid overshadowing material). Max bonus ~30.
static const int WHITE_PAWN_POSITION_TABLE[64] = {
    0, 0, 0, 0, 0, 0, 0, 0,                  // Rank 1
    0, 0, 0, 0, 0, 0, 0, 0,                  // Rank 2
    4, 4, 5, 5, 5, 5, 4, 4,                  // Rank 3
    6, 6, 8, 10,10, 8, 6, 6,                 // Rank 4
    10,10,14,16,16,14,10,10,                 // Rank 5
    12,12,18,22,22,18,12,12,                 // Rank 6
    20,20,20,22,22,20,20,20,                 // Rank 7
    0, 0, 0, 0, 0, 0, 0, 0                   // Rank 8 (promotion gives material; no extra)
};

static const int BLACK_PAWN_POSITION_TABLE[64] = {
    0, 0, 0, 0, 0, 0, 0, 0,                  // Rank 1 (promotion rank for Black skipped bonus)
    20,20,20,22,22,20,20,20,                 // Rank 2
    12,12,18,22,22,18,12,12,                 // Rank 3
    10,10,14,16,16,14,10,10,                 // Rank 4
    6, 6, 8, 10,10, 8, 6, 6,                 // Rank 5
    4, 4, 5, 5, 5, 5, 4, 4,                  // Rank 6
    0, 0, 0, 0, 0, 0, 0, 0,                  // Rank 7
    0, 0, 0, 0, 0, 0, 0, 0                   // Rank 8 (start)
};



static const int BLACK_KNIGHT_POSITION_TABLE[64] = {
    -30,-25,-20,-20,-20,-20,-25,-30,
    -15,-10, -5, -5, -5, -5,-10,-15,
    -10,  0, 10, 12, 12, 10,  0,-10,
    -10,  5, 15, 18, 18, 15,  5,-10,
    -10,  5, 15, 18, 18, 15,  5,-10,
    -10,  0, 10, 12, 12, 10,  0,-10,
    -15,-10, -5, -5, -5, -5,-10,-15,
    -30,-25,-20,-20,-20,-20,-25,-30
};

static const int WHITE_KNIGHT_POSITION_TABLE[64] = {
    -30,-25,-20,-20,-20,-20,-25,-30,
    -15,-10, -5, -5, -5, -5,-10,-15,
    -10,  0, 10, 12, 12, 10,  0,-10,
    -10,  5, 15, 18, 18, 15,  5,-10,
    -10,  5, 15, 18, 18, 15,  5,-10,
    -10,  0, 10, 12, 12, 10,  0,-10,
    -15,-10, -5, -5, -5, -5,-10,-15,
    -30,-25,-20,-20,-20,-20,-25,-30
};


// Bishop position table - bishops prefer long diagonals
static const int BLACK_BISHOP_POSITION_TABLE[64] = {
     0,  0,  0,  0,  0,  0,  0,  0,
     0,  6,  6,  6,  6,  6,  6,  0,
     0,  8, 10, 10, 10, 10,  8,  0,
     0, 10, 14, 16, 16, 14, 10,  0,
     0, 10, 14, 16, 16, 14, 10,  0,
     0,  8, 10, 10, 10, 10,  8,  0,
     0,  6,  6,  6,  6,  6,  6,  0,
     0,  0,  0,  0,  0,  0,  0,  0
};

static const int WHITE_BISHOP_POSITION_TABLE[64] = {
     0,  0,  0,  0,  0,  0,  0,  0,
     0,  6,  6,  6,  6,  6,  6,  0,
     0,  8, 10, 10, 10, 10,  8,  0,
     0, 10, 14, 16, 16, 14, 10,  0,
     0, 10, 14, 16, 16, 14, 10,  0,
     0,  8, 10, 10, 10, 10,  8,  0,
     0,  6,  6,  6,  6,  6,  6,  0,
     0,  0,  0,  0,  0,  0,  0,  0
};

// Rook position table - rooks are flexible
static const int BLACK_ROOK_POSITION_TABLE[64] = {
    0,  0,  2,  4,  4,  2,  0,  0,
    0,  2,  4,  6,  6,  4,  2,  0,
    0,  2,  4,  6,  6,  4,  2,  0,
    0,  4,  6,  8,  8,  6,  4,  0,
    0,  4,  6,  8,  8,  6,  4,  0,
    0,  2,  4,  6,  6,  4,  2,  0,
    0,  2,  2,  4,  4,  2,  2,  0,
    0,  0,  0,  0,  0,  0,  0,  0
};

static const int WHITE_ROOK_POSITION_TABLE[64] = {
    0,  0,  2,  4,  4,  2,  0,  0,
    0,  2,  4,  6,  6,  4,  2,  0,
    0,  2,  4,  6,  6,  4,  2,  0,
    0,  4,  6,  8,  8,  6,  4,  0,
    0,  4,  6,  8,  8,  6,  4,  0,
    0,  2,  4,  6,  6,  4,  2,  0,
    0,  2,  2,  4,  4,  2,  2,  0,
    0,  0,  0,  0,  0,  0,  0,  0
};

// Queen position table - queens are powerful anywhere
static const int BLACK_QUEEN_POSITION_TABLE[64] = {
    -8, -6, -4, -2, -2, -4, -6, -8,
    -4, -2,  0,  2,  2,  0, -2, -4,
    -2,  0,  2,  4,  4,  2,  0, -2,
    -2,  2,  4,  6,  6,  4,  2, -2,
    -2,  2,  4,  6,  6,  4,  2, -2,
    -2,  0,  2,  4,  4,  2,  0, -2,
    -4, -2,  0,  2,  2,  0, -2, -4,
    -8, -6, -4, -2, -2, -4, -6, -8
};

static const int WHITE_QUEEN_POSITION_TABLE[64] = {
    -8, -6, -4, -2, -2, -4, -6, -8,
    -4, -2,  0,  2,  2,  0, -2, -4,
    -2,  0,  2,  4,  4,  2,  0, -2,
    -2,  2,  4,  6,  6,  4,  2, -2,
    -2,  2,  4,  6,  6,  4,  2, -2,
    -2,  0,  2,  4,  4,  2,  0, -2,
    -4, -2,  0,  2,  2,  0, -2, -4,
    -8, -6, -4, -2, -2, -4, -6, -8
};

// King position tables (middlegame-oriented)
// Layout reminder: H1=0, G1=1, F1=2, ... A1=7, H2=8, ... A8=63
// Philosophy:
//  - Modest bonus for castled squares (g1/c1 for White, g8/c8 for Black)
//  - Small bonus for corners, penalty toward center and for advancing the king
//  - Symmetric across files; Black is rank-mirrored version of White
static const int WHITE_KING_POSITION_TABLE[64] = {
    10, 12,  8, -4,  8, 12, 10, 10,
     4,  6,  2, -6, -6,  2,  6,  4,
    -4, -6, -8,-12,-12, -8, -6, -4,
    -8,-10,-12,-16,-16,-12,-10, -8,
    -8,-10,-12,-16,-16,-12,-10, -8,
   -10,-12,-14,-18,-18,-14,-12,-10,
   -12,-14,-16,-20,-20,-16,-14,-12,
   -14,-16,-18,-22,-22,-18,-16,-14
};

static const int BLACK_KING_POSITION_TABLE[64] = {
   -14,-16,-18,-22,-22,-18,-16,-14,
   -12,-14,-16,-20,-20,-16,-14,-12,
   -10,-12,-14,-18,-18,-14,-12,-10,
    -8,-10,-12,-16,-16,-12,-10, -8,
    -8,-10,-12,-16,-16,-12,-10, -8,
    -4, -6, -8,-12,-12, -8, -6, -4,
     4,  6,  2, -6, -6,  2,  6,  4,
    10, 12,  8, -4,  8, 12, 10, 10
};

// ============================================================================
// TEMPORARY TEST TABLES - ALL ZEROS TO ISOLATE POSITION TABLE ISSUES
// Comment out the #define below to use original tables above
// ============================================================================
/*
#define USE_TEST_TABLES

#ifdef USE_TEST_TABLES
// Override all tables with zeros
#undef WHITE_PAWN_POSITION_TABLE
#undef BLACK_PAWN_POSITION_TABLE
#undef WHITE_KNIGHT_POSITION_TABLE
#undef BLACK_KNIGHT_POSITION_TABLE
#undef WHITE_BISHOP_POSITION_TABLE
#undef BLACK_BISHOP_POSITION_TABLE
#undef WHITE_ROOK_POSITION_TABLE
#undef BLACK_ROOK_POSITION_TABLE
#undef WHITE_QUEEN_POSITION_TABLE
#undef BLACK_QUEEN_POSITION_TABLE
#undef WHITE_KING_POSITION_TABLE
#undef BLACK_KING_POSITION_TABLE

static const int TEST_ZERO_TABLE[64] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0
};

#define WHITE_PAWN_POSITION_TABLE TEST_ZERO_TABLE
#define BLACK_PAWN_POSITION_TABLE TEST_ZERO_TABLE
#define WHITE_KNIGHT_POSITION_TABLE TEST_ZERO_TABLE
#define BLACK_KNIGHT_POSITION_TABLE TEST_ZERO_TABLE
#define WHITE_BISHOP_POSITION_TABLE TEST_ZERO_TABLE
#define BLACK_BISHOP_POSITION_TABLE TEST_ZERO_TABLE
#define WHITE_ROOK_POSITION_TABLE TEST_ZERO_TABLE
#define BLACK_ROOK_POSITION_TABLE TEST_ZERO_TABLE
#define WHITE_QUEEN_POSITION_TABLE TEST_ZERO_TABLE
#define BLACK_QUEEN_POSITION_TABLE TEST_ZERO_TABLE
#define WHITE_KING_POSITION_TABLE TEST_ZERO_TABLE
#define BLACK_KING_POSITION_TABLE TEST_ZERO_TABLE
#endif
*/
#endif // POSITION_TABLES_H
