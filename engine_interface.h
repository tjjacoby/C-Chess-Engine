// engine_interface.h
// C++ compatible interface for the chess engine

#ifndef ENGINE_INTERFACE_H
#define ENGINE_INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

// Simple move structure for communication
typedef struct {
    uint64_t from;
    uint64_t to;
} SimpleMove;

// Initialize the chess engine
void engine_init();

// Reset the board to starting position
void engine_reset();

// Set which color the player is (1 = white, 0 = black)
void engine_setPlayerColor(int isWhite);

// Try to make a move (returns 1 if valid, 0 if invalid)
int engine_makePlayerMove(uint64_t from, uint64_t to);

// Get AI's best move
SimpleMove engine_getAIMove();

// Get the current board state as bitboards
void engine_getBoardState(
    uint64_t* white_pawns, uint64_t* white_knights, uint64_t* white_bishops,
    uint64_t* white_rooks, uint64_t* white_queen, uint64_t* white_king,
    uint64_t* black_pawns, uint64_t* black_knights, uint64_t* black_bishops,
    uint64_t* black_rooks, uint64_t* black_queen, uint64_t* black_king
);

// Helper function to convert row,col to bitboard square
uint64_t engine_squareFromRowCol(int row, int col);

// Get the current evaluation score (positive = white advantage, negative = black advantage)
int engine_getCurrentScore();

#ifdef __cplusplus
}
#endif

#endif // ENGINE_INTERFACE_H
