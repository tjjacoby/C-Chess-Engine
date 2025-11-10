#ifndef POSITION_TABLES_H
#define POSITION_TABLES_H

// Piece-Square Tables for positional evaluation



// Layout: H1=0, G1=1, F1=2... A1=7, H2=8... A8=63
// Values scaled by 10 to make position matter more (0-30 instead of 0-3)

static const int PAWN_POSITION_TABLE[64] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 10, 10, 10, 10, 10, 10, 0,
    0, 10, 20, 20, 20, 20, 10, 0,
    0, 10, 20, 30, 30, 20, 10, 0,
    0, 10, 20, 30, 30, 20, 10, 0,
    0, 10, 20, 20, 20, 20, 10, 0,
    0, 10, 10, 10, 10, 10, 10, 0,
    0, 0, 0, 0, 0, 0, 0, 0
};

static const int KNIGHT_POSITION_TABLE[64] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 10, 10, 10, 10, 10, 10, 0,
    0, 10, 20, 20, 20, 20, 10, 0,
    0, 10, 20, 30, 30, 20, 10, 0,
    0, 10, 20, 30, 30, 20, 10, 0,
    0, 10, 20, 20, 20, 20, 10, 0,
    0, 10, 10, 10, 10, 10, 10, 0,
    0, 0, 0, 0, 0, 0, 0, 0
};

// Bishop position table - bishops prefer long diagonals
static const int BISHOP_POSITION_TABLE[64] = {
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
static const int ROOK_POSITION_TABLE[64] = {
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
static const int QUEEN_POSITION_TABLE[64] = {
    -10, -10, 0, 0, 0, 0, -10, -10,
    0, 10, 10, 10, 10, 10, 10, 0,
    0, 10, 20, 20, 20, 20, 10, 0,
    0, 10, 20, 30, 30, 20, 10, 0,
    0, 10, 20, 30, 30, 20, 10, 0,
    0, 10, 20, 20, 20, 20, 10, 0,
    0, 10, 10, 10, 10, 10, 10, 0,
    -10, -10, 0, 0, 0, 0, -10, -10
};

// King position table - kings prefer safety on edges/corners
static const int KING_POSITION_TABLE[64] = {
    30, 20, 20, 20, 20, 20, 20, 30,
    20, 10, 10, 10, 10, 10, 10, 20,
    20, 10, 0, 0, 0, 0, 10, 20,
    20, 10, 0, 0, 0, 0, 10, 20,
    20, 10, 0, 0, 0, 0, 10, 20,
    20, 10, 0, 0, 0, 0, 10, 20,
    20, 10, 10, 10, 10, 10, 10, 20,
    30, 20, 20, 20, 20, 20, 20, 30
};

#endif // POSITION_TABLES_H
