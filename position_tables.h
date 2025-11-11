#ifndef POSITION_TABLES_H
#define POSITION_TABLES_H

// Piece-Square Tables for positional evaluation



// Layout: H1=0, G1=1, F1=2... A1=7, H2=8... A8=63
// Values encourage pawn advancement toward promotion
// White pawns: start rank 2, promote rank 8
// Black pawns: start rank 7, promote rank 8

static const int WHITE_PAWN_POSITION_TABLE[64] = {
    0, 0, 0, 0, 0, 0, 0, 0,                  // Rank 1 - White starts here (neutral)
    0, 0, 0, 0, 0, 0, 0, 0,                  // Rank 2 - White starting rank (neutral)
    5, 5, 5, 5, 5, 5, 5, 5,                  // Rank 3 - small bonus for advancing
    5, 5, 10, 10, 10, 10, 5, 5,              // Rank 4 - better in center
    10, 10, 20, 25, 25, 20, 10, 10,          // Rank 5 - good advance
    10, 10, 20, 30, 30, 20, 10, 10,          // Rank 6 - near promotion
    50, 50, 50, 50, 50, 50, 50, 50,          // Rank 7 - one from promotion
    90, 90, 90, 90, 90, 90, 90, 90           // Rank 8 - White promotes here
};

static const int BLACK_PAWN_POSITION_TABLE[64] = {
    90, 90, 90, 90, 90, 90, 90, 90,          // Rank 1 - Black promotes here
    50, 50, 50, 50, 50, 50, 50, 50,          // Rank 2 - one from promotion
    10, 10, 20, 30, 30, 20, 10, 10,          // Rank 3 - near promotion
    10, 10, 20, 25, 25, 20, 10, 10,          // Rank 4 - good advance
    5, 5, 10, 10, 10, 10, 5, 5,              // Rank 5 - better in center
    5, 5, 5, 5, 5, 5, 5, 5,                  // Rank 6 - small bonus for advancing
    0, 0, 0, 0, 0, 0, 0, 0,                  // Rank 7 - Black starting rank (neutral)
    0, 0, 0, 0, 0, 0, 0, 0                   // Rank 8 - (neutral)
};



static const int BLACK_KNIGHT_POSITION_TABLE[64] = {
    -50, -40, -40, -40, -40, -40, -40, -50,
    -10, -20, 0, 0, 0, 0, -20, -10,
    -10, 0, 10, 20, 20, 10, 0, -10,
    -10, 0, 20, 25, 25, 20, 0, -10,
    -10, 0, 20, 25, 25, 20, 0, -10,
    -10, 0, 10, 20, 20, 10, 0, -10,
    -10, -20, 0, 0, 0, 0, -20, -10,
    -50, -40, -40, -40, -40, -40, -40, -50
};



static const int WHITE_KNIGHT_POSITION_TABLE[64] = {
    -50, -40, -40, -40, -40, -40, -40, -50,
    -10, -20, 0, 0, 0, 0, -20, -10,
    -10, 0, 10, 20, 20, 10, 0, -10,
    -10, 0, 20, 25, 25, 20, 0, -10,
    -10, 0, 20, 25, 25, 20, 0, -10,
    -10, 0, 10, 20, 20, 10, 0, -10,
    -10, -20, 0, 0, 0, 0, -20, -10,
    -50, -40, -40, -40, -40, -40, -40, -50
};


// Bishop position table - bishops prefer long diagonals
static const int BLACK_BISHOP_POSITION_TABLE[64] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 10, 10, 10, 10, 10, 10, 0,
    0, 10, 20, 20, 20, 20, 10, 0,
    0, 10, 20, 30, 30, 20, 10, 0,
    0, 10, 20, 30, 30, 20, 10, 0,
    0, 10, 20, 20, 20, 20, 10, 0,
    0, 10, 10, 10, 10, 10, 10, 0,
    0, 0, 0, 0, 0, 0, 0, 0
};

static const int WHITE_BISHOP_POSITION_TABLE[64] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 10, 10, 10, 10, 10, 10, 0,
    0, 10, 20, 20, 20, 20, 10, 0,
    0, 10, 20, 30, 30, 20, 10, 0,
    0, 10, 20, 30, 30, 20, 10, 0,
    0, 10, 20, 20, 20, 20, 10, 0,
    0, 10, 10, 10, 10, 10, 10, 0,
    0, 0, 0, 0, 0, 0, 0, 0
};

// Rook position table - rooks are flexible
static const int BLACK_ROOK_POSITION_TABLE[64] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 10, 10, 10, 10, 10, 10, 0,
    0, 10, 20, 20, 20, 20, 10, 0,
    0, 10, 20, 30, 30, 20, 10, 0,
    0, 10, 20, 30, 30, 20, 10, 0,
    0, 10, 20, 20, 20, 20, 10, 0,
    0, 10, 10, 10, 10, 10, 10, 0,
    0, 0, 0, 0, 0, 0, 0, 0
};

static const int WHITE_ROOK_POSITION_TABLE[64] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 10, 10, 10, 10, 10, 10, 0,
    0, 10, 20, 20, 20, 20, 10, 0,
    0, 10, 20, 30, 30, 20, 10, 0,
    0, 10, 20, 30, 30, 20, 10, 0,
    0, 10, 20, 20, 20, 20, 10, 0,
    0, 10, 10, 10, 10, 10, 10, 0,
    0, 0, 0, 0, 0, 0, 0, 0
};

// Queen position table - queens are powerful anywhere
static const int BLACK_QUEEN_POSITION_TABLE[64] = {
    -10, -10, 0, 0, 0, 0, -10, -10,
    0, 10, 10, 10, 10, 10, 10, 0,
    0, 10, 20, 20, 20, 20, 10, 0,
    0, 10, 20, 30, 30, 20, 10, 0,
    0, 10, 20, 30, 30, 20, 10, 0,
    0, 10, 20, 20, 20, 20, 10, 0,
    0, 10, 10, 10, 10, 10, 10, 0,
    -10, -10, 0, 0, 0, 0, -10, -10
};

static const int WHITE_QUEEN_POSITION_TABLE[64] = {
    -10, -10, 0, 0, 0, 0, -10, -10,
    0, 10, 10, 10, 10, 10, 10, 0,
    0, 10, 20, 20, 20, 20, 10, 0,
    0, 10, 20, 30, 30, 20, 10, 0,
    0, 10, 20, 30, 30, 20, 10, 0,
    0, 10, 20, 20, 20, 20, 10, 0,
    0, 10, 10, 10, 10, 10, 10, 0,
    -10, -10, 0, 0, 0, 0, -10, -10
};

// King position tables (middlegame-oriented)
// Layout reminder: H1=0, G1=1, F1=2, ... A1=7, H2=8, ... A8=63
// Philosophy:
//  - Modest bonus for castled squares (g1/c1 for White, g8/c8 for Black)
//  - Small bonus for corners, penalty toward center and for advancing the king
//  - Symmetric across files; Black is rank-mirrored version of White
static const int WHITE_KING_POSITION_TABLE[64] = {
    // Rank 1  (H1 .. A1)
    20, 30, 10, -10, 10, 30, 10, 20,
    // Rank 2
     0,  0,  -5, -10, -10,  -5,  0,  0,
    // Rank 3
    -10, -10, -20, -25, -25, -20, -10, -10,
    // Rank 4
    -20, -25, -30, -35, -35, -30, -25, -20,
    // Rank 5
    -20, -25, -30, -35, -35, -30, -25, -20,
    // Rank 6
    -25, -30, -35, -40, -40, -35, -30, -25,
    // Rank 7
    -30, -35, -40, -45, -45, -40, -35, -30,
    // Rank 8
    -40, -45, -50, -55, -55, -50, -45, -40
};

// Black is a rank-mirrored version of White (favors castling on rank 8)
static const int BLACK_KING_POSITION_TABLE[64] = {
    // Rank 1  (H1 .. A1)
    -40, -45, -50, -55, -55, -50, -45, -40,
    // Rank 2
    -30, -35, -40, -45, -45, -40, -35, -30,
    // Rank 3
    -25, -30, -35, -40, -40, -35, -30, -25,
    // Rank 4
    -20, -25, -30, -35, -35, -30, -25, -20,
    // Rank 5
    -20, -25, -30, -35, -35, -30, -25, -20,
    // Rank 6
    -10, -10, -20, -25, -25, -20, -10, -10,
    // Rank 7
     0,   0,  -5, -10, -10,  -5,   0,   0,
    // Rank 8
    20,  30,  10, -10,  10,  30,  10,  20
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
