// engine_interface.c
// Implementation of the C++ compatible interface

#include "engine_interface.h"
#include "globals.h"
#include "moves.h"
#include <stdio.h>
#include <string.h>  // For memset
#include <stdlib.h>  // For free

// External declarations from main.c
extern uint64_t White_king, Black_king, White_rooks, Black_rooks, White_queen, Black_queen;
extern uint64_t White_pawns, Black_pawns, White_knights, Black_knights, White_bishops, Black_bishops;
extern uint64_t White, Black, Main;
extern int WcastleL, WcastleR, BcastleL, BcastleR;

// External functions from main.c
extern void initPieceBitboards();
extern void updateAll();
extern int isCheck(int isWhite);

// Track which color the player is (1 = white, 0 = black)
static int playerIsWhite = 1;

// Track the current evaluation score
static int currentScore = 0;

// Helper function to convert bitboard position to chess notation
const char* bitboardToSquareName(uint64_t square) {
    // Map each bitboard position to its square name
    if (square == A1) return "A1";
    if (square == B1) return "B1";
    if (square == C1) return "C1";
    if (square == D1) return "D1";
    if (square == E1) return "E1";
    if (square == F1) return "F1";
    if (square == G1) return "G1";
    if (square == H1) return "H1";
    
    if (square == A2) return "A2";
    if (square == B2) return "B2";
    if (square == C2) return "C2";
    if (square == D2) return "D2";
    if (square == E2) return "E2";
    if (square == F2) return "F2";
    if (square == G2) return "G2";
    if (square == H2) return "H2";
    
    if (square == A3) return "A3";
    if (square == B3) return "B3";
    if (square == C3) return "C3";
    if (square == D3) return "D3";
    if (square == E3) return "E3";
    if (square == F3) return "F3";
    if (square == G3) return "G3";
    if (square == H3) return "H3";
    
    if (square == A4) return "A4";
    if (square == B4) return "B4";
    if (square == C4) return "C4";
    if (square == D4) return "D4";
    if (square == E4) return "E4";
    if (square == F4) return "F4";
    if (square == G4) return "G4";
    if (square == H4) return "H4";
    
    if (square == A5) return "A5";
    if (square == B5) return "B5";
    if (square == C5) return "C5";
    if (square == D5) return "D5";
    if (square == E5) return "E5";
    if (square == F5) return "F5";
    if (square == G5) return "G5";
    if (square == H5) return "H5";
    
    if (square == A6) return "A6";
    if (square == B6) return "B6";
    if (square == C6) return "C6";
    if (square == D6) return "D6";
    if (square == E6) return "E6";
    if (square == F6) return "F6";
    if (square == G6) return "G6";
    if (square == H6) return "H6";
    
    if (square == A7) return "A7";
    if (square == B7) return "B7";
    if (square == C7) return "C7";
    if (square == D7) return "D7";
    if (square == E7) return "E7";
    if (square == F7) return "F7";
    if (square == G7) return "G7";
    if (square == H7) return "H7";
    
    if (square == A8) return "A8";
    if (square == B8) return "B8";
    if (square == C8) return "C8";
    if (square == D8) return "D8";
    if (square == E8) return "E8";
    if (square == F8) return "F8";
    if (square == G8) return "G8";
    if (square == H8) return "H8";
    
    return "Unknown";
}

// Helper function to identify piece type at a given position
const char* getPieceTypeName(uint64_t square) {
    if (square & White_pawns) return "White Pawn";
    if (square & White_knights) return "White Knight";
    if (square & White_bishops) return "White Bishop";
    if (square & White_rooks) return "White Rook";
    if (square & White_queen) return "White Queen";
    if (square & White_king) return "White King";
    if (square & Black_pawns) return "Black Pawn";
    if (square & Black_knights) return "Black Knight";
    if (square & Black_bishops) return "Black Bishop";
    if (square & Black_rooks) return "Black Rook";
    if (square & Black_queen) return "Black Queen";
    if (square & Black_king) return "Black King";
    return "Empty/Unknown";
}

void engine_init() {
    initPieceBitboards();
    printf("Chess engine initialized\n");
}

void engine_reset() {
    reset();
    updateAll();
}

void engine_setPlayerColor(int isWhite) {
    playerIsWhite = isWhite;
    printf("Player color set to: %s\n", isWhite ? "White" : "Black");
}

int engine_makePlayerMove(uint64_t from, uint64_t to) {
    Move move;
    memset(&move, 0, sizeof(Move));  // Zero out the entire structure
    move.Piece = from;
    move.Square = to;
    move.score = 0;
    
    // Force update of White, Black, and Main before making move
    extern void updateAll();
    extern uint64_t White, Black;
    updateAll();

    // Check if there's actually a player piece at 'from'
    uint64_t playerPieces = playerIsWhite ? White : Black;
    if (!(playerPieces & from)) {
        printf("ERROR: No %s piece at from square!\n",
               playerIsWhite ? "white" : "black");
        return 0;
    }

    // Identify the piece type and square names
    const char* pieceType = getPieceTypeName(from);
    const char* fromSquare = bitboardToSquareName(from);
    const char* toSquare = bitboardToSquareName(to);

    int result = makeMove(move, playerIsWhite); // Use player's actual color
    
    if (result == 1) {
        printf("Player move valid: %s from %s to %s\n", pieceType, fromSquare, toSquare);
        return 1;
    } else {
        printf("Player move INVALID: %s from %s to %s (error code: %d)\n", pieceType, fromSquare, toSquare, result);
        return 0;
    }
}

SimpleMove engine_getAIMove() {
    printf("AI is thinking...\n");
    
    // Force update before AI calculates
    updateAll();
    
    // DEBUG: Check piece counts before AI search
    extern int countSetBits(uint64_t);
    // AI is opposite color of player
    int aiColor = playerIsWhite ? 0 : 1; // If player is white (1), AI is black (0), and vice versa
    Move aiMove = bestMove(aiColor);
    
    // Store the evaluation score
    currentScore = aiMove.score;
    
    SimpleMove result;
    result.from = aiMove.Piece;
    result.to = aiMove.Square;

    if (aiMove.Piece != 0 || aiMove.Square != 0) {
        // Get piece info BEFORE making the move
        const char* pieceType = getPieceTypeName(result.from);
        const char* fromSquare = bitboardToSquareName(result.from);
        const char* toSquare = bitboardToSquareName(result.to);

        int moveResult = makeMove(aiMove, aiColor); // Use AI's actual color
        if (moveResult == 1) {
            printf("AI moved: %s from %s to %s\n", pieceType, fromSquare, toSquare);
        } else {
            printf("AI move FAILED: %s from %s to %s (error code: %d)\n", 
                   pieceType, fromSquare, toSquare, moveResult);
        }
    } else {
        printf("AI has no legal moves!\n");
    }
    
    return result;
}

void engine_getBoardState(
    uint64_t* white_pawns, uint64_t* white_knights, uint64_t* white_bishops,
    uint64_t* white_rooks, uint64_t* white_queen, uint64_t* white_king,
    uint64_t* black_pawns, uint64_t* black_knights, uint64_t* black_bishops,
    uint64_t* black_rooks, uint64_t* black_queen, uint64_t* black_king
) {
    *white_pawns = White_pawns;
    *white_knights = White_knights;
    *white_bishops = White_bishops;
    *white_rooks = White_rooks;
    *white_queen = White_queen;
    *white_king = White_king;
    
    *black_pawns = Black_pawns;
    *black_knights = Black_knights;
    *black_bishops = Black_bishops;
    *black_rooks = Black_rooks;
    *black_queen = Black_queen;
    *black_king = Black_king;
}

// Helper to convert row,col (0-7) to bitboard position
// In the GUI: Row 0 = top of screen (rank 8), Row 7 = bottom of screen (rank 1)
// In the GUI: Col 0 = left (file A), Col 7 = right (file H)
// In bitboards: Rank 1 at bottom, Rank 8 at top
// Bitboards go H1 (bit 0) -> A1 (bit 7) -> H2 (bit 8) -> ... -> A8 (bit 63)
uint64_t engine_squareFromRowCol(int row, int col) {
    // Convert GUI row to chess rank (flip vertically)
    int rank = 7 - row;  // row 0 (top) -> rank 7 (8th rank), row 7 (bottom) -> rank 0 (1st rank)
    
    // Convert GUI col to file (reverse horizontally because bitboard goes H->A)
    int file = 7 - col;  // col 0 (left/A) -> file 7, col 7 (right/H) -> file 0
    
    //return  1ULL << ((rank - 1) * 8 + (file - 'A'));
    // Calculate bit position: rank * 8 + file
    int bitPos = rank * 8 + file;
    return 1ULL << bitPos;
}

int engine_getCurrentScore() {
    return currentScore;
}

int engine_getGameStatus(int sideToMoveIsWhite) {
    updateAll();

    // findMoves returns only fully legal moves (already filtered for checks)
    MoveList moves = findMoves(sideToMoveIsWhite);
    int legalCount = moves.size;
    if (moves.moves) free(moves.moves);

    if (legalCount > 0) {
        return 0; // game ongoing
    }

    // No legal moves: in check => checkmate, otherwise => stalemate
    return isCheck(sideToMoveIsWhite) ? 1 : 2;
}
