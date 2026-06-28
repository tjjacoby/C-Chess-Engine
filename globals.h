// globals.h
#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdint.h>

// BoardState structure for storing game state
typedef struct {
    uint64_t White_pawns;
    uint64_t White_knights;
    uint64_t White_rooks;
    uint64_t White_bishops;
    uint64_t White_queen;
    uint64_t White_king;
    uint64_t Black_pawns;
    uint64_t Black_knights;
    uint64_t Black_rooks;
    uint64_t Black_bishops;
    uint64_t Black_queen;
    uint64_t Black_king;
} BoardState;

// Move structure for representing a chess move
typedef struct {
    uint64_t Bwhite; // White pieces bitboard 
    uint64_t Bblack; // Black pieces bitboard
    uint64_t Piece; // The piece being moved (bitboard)
    uint64_t Square; // The destination square (bitboard)
    int type; // Type of piece (1=pawn, 2=knight, 3=bishop, 4=rook, 5=queen, 6=king)
    int capturetype; // Type of piece captured (if any)
    int score; // Evaluation score after the move
    int castle; // Castling rights after the move (0=none, 1=king-side, 2=queen-side)
} Move;

// Entry in move history retains the previous board state for undo operations.
typedef struct {
    Move move;
    BoardState prevState;
} MoveHistoryEntry;

// List of all moves
typedef struct {
    Move *moves;
    int size;
} MoveList;

// Global variables
extern uint64_t White_king, Black_king, White_rooks, Black_rooks, White_queen, Black_queen;
extern uint64_t White_pawns, Black_pawns;
extern int WcastleL, WcastleR, BcastleL, BcastleR;


// Function prototypes
void initPieceBitboards();
void reset();
void updateAll();
Move bestMove(int isWhite);
MoveList findMoves(int isWhite);
int makeMove(Move move, int isWhite);
int unmakeMove();

#endif // GLOBALS_H
