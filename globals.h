// globals.h
#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdint.h>

// BoardState structure for storing game state
typedef struct {
    uint64_t White_ponds;
    uint64_t White_knights;
    uint64_t White_rooks;
    uint64_t White_bishops;
    uint64_t White_queen;
    uint64_t White_king;
    uint64_t Black_ponds;
    uint64_t Black_knights;
    uint64_t Black_rooks;
    uint64_t Black_bishops;
    uint64_t Black_queen;
    uint64_t Black_king;
} BoardState;

// Define shared structs - MUST match the definition in main.c
typedef struct {
    uint64_t Bwhite;
    uint64_t Bblack;
    uint64_t Piece;
    uint64_t Square;
    int type;
    int capturetype;
    int score;
    int castle;
    BoardState prevState;
} Move;

typedef struct {
    Move *moves;
    int size;
} MoveList;

// Global variables
extern uint64_t White_king, Black_king, White_rooks, Black_rooks, White_queen, Black_queen;
extern uint64_t White_ponds, Black_ponds;
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
