// globals.h
#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdint.h>

// Define shared structs
typedef struct {
    uint64_t Piece;
    uint64_t Square;
    int score;
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
