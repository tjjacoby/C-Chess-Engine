#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "globals.h"
#include "moves.h"
#include "binary_printer.h"
#include "position_tables.h"


#define MAX_MOVES 65536  // Limit move history to a safe bound 
#define BASE_DEPTH 4   // Base depth for midgame ( 4 is fast and good, 6 is slower plays around ~2300 elo)
#define QMAX_DEPTH 4   // Maximum depth for quiescence capture extension (reduced for speed)


// A BitBoard is just a 64-bit number used to represent the chessboard
typedef uint64_t BitBoard;

BitBoard vaildMovesWhite(void);
BitBoard vaildMovesBlack(void);
BitBoard find_allrookmoves(BitBoard Piece, int isWhite);
BitBoard find_allbishopmoves(BitBoard Piece, int isWhite);

static inline BitBoard attacks_by_white(void);
static inline BitBoard attacks_by_black(void);
static inline int is_square_attacked(BitBoard square, int byWhite);

MoveList combineMoveLists(MoveList list1, MoveList list2);
MoveList find_Castles(int isWhite);
Move castlemove(BitBoard Piece,int isWhite);
BitBoard find_all_attacks(int isWhite);
MoveList filterChecks(MoveList moves,int isWhite);
int unmakeMove();
int makeMove(Move move,int isWhite);
int move_piece(Move move, int isWhite);
int isCheck(int isWhite);
MoveList findMoves(int isWhite);
int quiesce(int alpha, int beta, int isMaximizingPlayer, int qdepth);

void updateAll();

// Castling flags
int WcastleL = 1;
int WcastleR = 1;
int BcastleL = 1;
int BcastleR = 1;

// infinite values for alpha-beta pruning
#define INF 2147483647
#define NINF -2147483647 

// Piece values, pawns are base unit
#define pawnValue 100
#define RookValue 500
#define BishopValue 320
#define QueenValue 900
#define KnightValue 320

// List of all moves made in the game
MoveHistoryEntry MoveHistory[MAX_MOVES];

//Init Main Boards, all of the white pieces,black pieces, and all pieces
BitBoard Main;
BitBoard White;
BitBoard Black;

//Init White BitBoards
BitBoard White_pawns;
BitBoard White_knights;
BitBoard White_rooks; 
BitBoard White_bishops;
BitBoard White_queen;
BitBoard White_king;

//Init Black BitBoards
BitBoard Black_pawns;
BitBoard Black_knights;
BitBoard Black_rooks; 
BitBoard Black_bishops;
BitBoard Black_queen;
BitBoard Black_king;

int moveCount = 0;
int movesEvaluated = 0;  // Counter for moves evaluated during search

int count;

// Root search depth (used to prefer faster mates)
static int rootSearchDepth = 0;

typedef struct {
    Move bestMove;
    int score;
} MoveScore;


int find_type(BitBoard piece) {

    if((piece & Main) == 0) {
        return 0;
    }
    // Check for pawn
        if (piece & (White_pawns | Black_pawns)) {
        return Typepawn; 
    }
    
    // Check for knight
    if (piece & (White_knights | Black_knights)) {
        return Typeknight; 
    }

    // Check for bishop
    if (piece & (White_bishops | Black_bishops)) {
        return Typebishop; 
    }

    // Check for rook
    if (piece & (White_rooks | Black_rooks)) {
        return Typerook; 
    }

    // Check for queen
    if (piece & (White_queen | Black_queen)) {
        return Typequeen; 
    }

    // Check for king
    if (piece & (White_king | Black_king)) {
        return Typeking;
    }

    // Default case: fails
    return -1;
}

// Piece value helper (centipawns) for ordering and evaluation adjuncts
static inline int piece_value_from_type(int t) {
    switch(t) {
        case Typepawn: return pawnValue;   // Pawn
        case Typeknight: return KnightValue;   // Knight
        case Typebishop: return BishopValue;   // Bishop
        case Typerook: return RookValue;   // Rook
        case Typequeen: return QueenValue;   // Queen
        case Typeking: return 0;     // King (not used for MVV-LVA ordering)
        default: return 0;
    }
}

// Quick piece value (king large for safety heuristic if needed later)
static inline int full_piece_value(int type) {
    switch(type) {
        case 1: return pawnValue; case 2: return KnightValue; case 3: return BishopValue; case 4: return RookValue; case 5: return QueenValue; case 6: return 20000; default: return 0;
    }
}
// Approximate attackers: if side's moves cover the square
static inline int square_is_attacked(BitBoard square, int byWhite) {
    if (byWhite) {
        return (vaildMovesWhite() & square) ? 1 : 0;
    } else {
        return (vaildMovesBlack() & square) ? 1 : 0;
    }
}

// Approximate defenders: if side's moves cover the square OR a same-color pawn directly defends it (simplistic).
static inline int square_is_defended(BitBoard square, int byWhite) {
    if (byWhite) {
        return (vaildMovesWhite() & square) ? 1 : 0;
    } else {
        return (vaildMovesBlack() & square) ? 1 : 0;
    }
}

// Hanging penalty: if destination square is attacked by opponent and not defended , apply half piece value penalty.
static inline int hanging_penalty(BitBoard pieceBB, BitBoard destBB, int moverIsWhite) {
    int type = find_type(pieceBB);
    if (type == 0) return 0;
    if (type == Typeking) return 0;
    int attacked = square_is_attacked(destBB, !moverIsWhite);
    if (!attacked) return 0;
    int defended = square_is_defended(destBB, moverIsWhite);
    if (defended) return 0;
    int val = full_piece_value(type);
    return val / 2; // half value penalty for hanging
}



// Used for debugging: print current castling rights
void printFlags() {
     printf("WL:%d\nWR:%d\nBL:%d\nBR:%d\n",WcastleL,WcastleR,BcastleL,BcastleR);
}
    

void updateMain(){
    Main = White | Black;
}

void updateWhite() {
White = White_pawns | White_knights | White_rooks | White_bishops | White_king | White_queen;

}
void updateBlack(){
    Black = Black_pawns | Black_knights | Black_rooks | Black_bishops | Black_king | Black_queen;
}
void updateAll() {
    updateBlack();
    updateWhite();
    updateMain();
}



// Compute pawn attack masks for a set of pawns
static inline BitBoard pawn_attacks(BitBoard pawns, int isWhite) {
    if (isWhite) {
        BitBoard left = (pawns & ClearFile_H) << 7; // toward H file
        BitBoard right = (pawns & ClearFile_A) << 9; // toward A file
        return left | right;
    } else {
        BitBoard left = (pawns & ClearFile_H) >> 9; // toward H file
        BitBoard right = (pawns & ClearFile_A) >> 7; // toward A file
        return left | right;
    }
}

// Knight attack mask for any number of knights (ignores own-occupancy masking)
static inline BitBoard knight_attacks(BitBoard knights) {
    BitBoard a = 0;
    BitBoard temp = knights;
    while (temp) {
        BitBoard k = getLSB(temp);
        temp = removeLSB(temp);

        BitBoard tall_L_right = (k & ClearFile_H) >> 17;
        BitBoard tall_L_left  = (k & ClearFile_A) >> 15;
        BitBoard short_L_right = (k & ClearFile_G & ClearFile_H) >> 10;
        BitBoard short_L_left  = (k & ClearFile_B & ClearFile_A) >> 6;
        BitBoard downTL_right  = (k & ClearFile_H) << 15;
        BitBoard downTL_left   = (k & ClearFile_A) << 17;
        BitBoard downShort_L_right = (k & ClearFile_G & ClearFile_H) << 6;
        BitBoard downshort_L_left  = (k & ClearFile_B & ClearFile_A) << 10;
        a |= tall_L_left | tall_L_right | short_L_left | short_L_right |
             downTL_right | downTL_left | downshort_L_left | downShort_L_right;
    }
    return a;
}

// King attack mask (no castling squares)
static inline BitBoard king_attacks(BitBoard king) {
    if (!king) return 0;
    BitBoard start_CA = king & ClearFile_A;
    BitBoard start_CH = king & ClearFile_H;
    BitBoard up   = king << 8;
    BitBoard down = king >> 8;
    BitBoard right = start_CA << 1;
    BitBoard left  = start_CH >> 1;
    BitBoard topR = start_CH << 7;
    BitBoard topL = start_CA << 9;
    BitBoard botR = start_CH >> 9;
    BitBoard botL = start_CA >> 7;
    return up | down | right | left | topL | topR | botL | botR;
}

// Aggregate attacks for each side (no castles, no pawn pushes)
static inline BitBoard attacks_by_white(void) {
    BitBoard a = 0;
    a |= pawn_attacks(White_pawns, 1);
    a |= knight_attacks(White_knights);
    a |= king_attacks(White_king);
    // Sliding pieces reuse existing generators per piece set
    a |= find_allrookmoves(White_rooks, 1);
    a |= find_allbishopmoves(White_bishops, 1);
    a |= (find_allrookmoves(White_queen, 1) | find_allbishopmoves(White_queen, 1));
    return a;
}

static inline BitBoard attacks_by_black(void) {
    BitBoard a = 0;
    a |= pawn_attacks(Black_pawns, 0);
    a |= knight_attacks(Black_knights);
    a |= king_attacks(Black_king);
    a |= find_allrookmoves(Black_rooks, 0);
    a |= find_allbishopmoves(Black_bishops, 0);
    a |= (find_allrookmoves(Black_queen, 0) | find_allbishopmoves(Black_queen, 0));
    return a;
}

static inline int is_square_attacked(BitBoard square, int byWhite) {
    return byWhite ? ((attacks_by_white() & square) != 0) : ((attacks_by_black() & square) != 0);
}

void reset() {
     White_pawns = A2 | B2 | C2 | D2 | E2 | F2 | G2 | H2;
    White_knights = B1 | G1;
    White_rooks = H1 | A1;
    White_bishops = C1 | F1;
    White_queen = D1;
    White_king = E1;


    Black_pawns = A7 | B7 | C7 | D7 | E7 | F7 | G7 | H7;
    Black_knights = B8 | G8;
    Black_rooks = H8 | A8;
    Black_bishops = C8 | F8;
    Black_queen = D8;
    Black_king = E8;
    WcastleL = 1;
    WcastleR = 1;
    BcastleL = 1;
    BcastleR = 1;  

    // Reset move history index to avoid unbounded growth across games
    count = 0;

    updateAll();
}
void initPieceBitboards() {
    White_pawns = A2 | B2 | C2 | D2 | E2 | F2 | G2 | H2;
    White_knights = B1 | G1;
    White_rooks = H1 | A1;
    White_bishops = C1 | F1;
    White_queen = D1;
    White_king = E1;


    Black_pawns = A7 | B7 | C7 | D7 | E7 | F7 | G7 | H7;
    Black_knights = B8 | G8;
    Black_rooks = H8 | A8;
    Black_bishops = C8 | F8;
    Black_queen = D8;
    Black_king = E8;

    

    updateAll();
    Move temp = (Move){0};
    temp.Bblack = Black;
    temp.Bwhite = White;
    MoveHistory[0].move = temp;
    MoveHistory[0].prevState.White_pawns = White_pawns;
    MoveHistory[0].prevState.White_knights = White_knights;
    MoveHistory[0].prevState.White_rooks = White_rooks;
    MoveHistory[0].prevState.White_bishops = White_bishops;
    MoveHistory[0].prevState.White_queen = White_queen;
    MoveHistory[0].prevState.White_king = White_king;
    MoveHistory[0].prevState.Black_pawns = Black_pawns;
    MoveHistory[0].prevState.Black_knights = Black_knights;
    MoveHistory[0].prevState.Black_rooks = Black_rooks;
    MoveHistory[0].prevState.Black_bishops = Black_bishops;
    MoveHistory[0].prevState.Black_queen = Black_queen;
    MoveHistory[0].prevState.Black_king = Black_king;
    
}

// https://www.chessprogramming.org/Hyperbola_Quintessence
BitBoard fileAttacks(BitBoard occ, BitBoard fileMaskEx, BitBoard bitMask) {
    // Uses o ^ (o -2r)

    BitBoard forward, reversed;

    forward = occ & fileMaskEx;

    reversed = reverse(forward); // Reverse the bits to handle the other direction (reversed as subtracting only works from low to high bits)
    
    forward -= bitMask; // Flips every bit between piece and edge or blocker
    reversed -= reverse(bitMask);  // just other way as subtracting only works in one direction

    forward ^= reverse(reversed); // merge the two directions
    
    forward &= fileMaskEx; // Gets rid of the extra bits that were flipped in the subtraction step
    
    return forward;
}

//Returns all vaild moves for a king
BitBoard find_kingmoves( BitBoard Piece, int isWhite) {
    
    BitBoard start = Piece;

    // Removed the piece on the sides of the boards
    BitBoard start_CA = start & ClearFile_A;
    BitBoard start_CH  = start & ClearFile_H;

    //Move up 
    BitBoard up = Piece << 8;
   
    //Move down 
    BitBoard down = Piece >> 8;

    //Move right

    BitBoard left = start_CA << 1;

    //Move left 
    BitBoard right = start_CH >> 1;

    //Move top left, right
    BitBoard topR = start_CH << 7;
    BitBoard topL = start_CA << 9;

    //Move bottom left, right
    BitBoard botR = start_CH >> 9;
    BitBoard botL = start_CA >> 7;
    BitBoard moves = 0;
    
    MoveList castles = find_Castles(isWhite);
    for(int i = 0; i < castles.size; i++) {
        if((castles.moves[i].castle) == 1) {
            moves |= C1;
        }
        if((castles.moves[i].castle) == 2) {
            moves |= G1;
        }
        if(castles.moves[i].castle == 3) {
            moves |= C8;
        }
        if(castles.moves[i].castle == 4) {
            moves |= G8;
        }
    }
    if (castles.moves) free(castles.moves);
        
      

    return (up|down|right|left|topL|topR|botL|botR|moves) & (isWhite ? ~White:~Black);

    }

// Finds and returns all valid pawn moves for a given pawn
BitBoard find_pawnmoves(BitBoard Piece, int isWhite) {

    BitBoard vaild_moves = ~(White|Black); 

    BitBoard moveOne = isWhite ? (Piece << 8) : (Piece >> 8);
    moveOne = moveOne & vaild_moves;

    BitBoard moveTwo = isWhite ? ((((Piece & Rank2) << 8 ) & (vaild_moves)) << 8) : ((((Piece & Rank7) >> 8 ) & (vaild_moves)) >> 8);
    moveTwo = moveTwo & vaild_moves;

    //CAPTURING

    BitBoard enemyColour = isWhite ? Black:White;

    // use the pawn's position (Piece) and avoid file wrap
    // In this bitboard: H=LSB (bit 0), A=MSB (bit 7) within each rank
    // For white: << 7 (toward H) needs ClearFile_H, << 9 (toward A) needs ClearFile_A
    // For black: >> 9 (toward A) needs ClearFile_A, >> 7 (toward H) needs ClearFile_H
    BitBoard takeLeft = isWhite ? (((Piece & ClearFile_H) << 7) & enemyColour)
                                : (((Piece & ClearFile_H) >> 9) & enemyColour);
    BitBoard takeRight = isWhite ? (((Piece & ClearFile_A) << 9) & enemyColour)
                                 : (((Piece & ClearFile_A) >> 7) & enemyColour);
    
    return moveOne|moveTwo|takeLeft|takeRight;

}

Move find_pawnmoveup(BitBoard Piece, int isWhite) {
    Move move;
    BitBoard vaild_moves = ~(White|Black); 
    move.Square = (isWhite ? (Piece << 8) : (Piece >> 8)) & vaild_moves;
    move.Piece = Piece;
    return move;

}
Move find_pawnmovedown(BitBoard Piece,int isWhite) {

    Move move;

    BitBoard vaild_moves = ~(White|Black);
    move.Square = (isWhite ? ((((Piece & Rank2) << 8 ) & (vaild_moves)) << 8) : ((((Piece & Rank7) >> 8 ) & (vaild_moves)) >> 8) )& vaild_moves;
    move.Piece = Piece;
    return move;
}
Move find_pawntakeright(BitBoard Piece, int isWhite) {

    Move move;

    BitBoard enemyColour = isWhite ? Black:White;

    // capture diagonally forward-right (toward A file in this bitboard)
    // In this bitboard: H=LSB (bit 0), A=MSB (bit 7) within each rank
    // For white: << 9 moves toward A, needs ClearFile_A to prevent wrap
    // For black: >> 7 moves toward A, needs ClearFile_A to prevent wrap

    move.Square = isWhite ? (((Piece & ClearFile_A) << 9) & enemyColour)
                          : (((Piece & ClearFile_A) >> 7) & enemyColour);
    move.Piece = Piece;

    return move;
}
Move find_pawntakeleft(BitBoard Piece, int isWhite) {
    Move move;
    BitBoard enemyColour = isWhite ? Black:White;
    // capture diagonally forward-left (toward H file in this bitboard)
    // In this bitboard: H=LSB (bit 0), A=MSB (bit 7) within each rank
    // For white: << 7 moves toward H, needs ClearFile_H to prevent wrap
    // For black: >> 9 moves toward H, needs ClearFile_H to prevent wrap

    move.Square = isWhite ? (((Piece & ClearFile_H) << 7) & enemyColour)
                          : (((Piece & ClearFile_H) >> 9) & enemyColour);
    move.Piece = Piece;

    return move;
}

// #1 Knight moves
BitBoard find_knightmoves(BitBoard Piece, int isWhite) {

    // Naming convention is from whites perspective, so "down" is toward rank 1, "up" is toward rank 8
    // Note works for black knights down is just up
    BitBoard downtall_L_right = (Piece & ClearFile_H) >> 17;
    BitBoard downtall_L_left = (Piece & ClearFile_A) >> 15;

    BitBoard downshort_L_right = (Piece & ClearFile_G & ClearFile_H) >> 10;
    BitBoard downshort_L_left = (Piece & ClearFile_B & ClearFile_A) >> 6;

    BitBoard uptall_L_right = (Piece & ClearFile_H) << 15;
    BitBoard uptall_L_left = (Piece & ClearFile_A) << 17;

    BitBoard upshort_L_right = (Piece & ClearFile_G & ClearFile_H) << 6;
    BitBoard upshort_L_left = (Piece & ClearFile_B & ClearFile_A) << 10;
    

    return (downtall_L_left|downtall_L_right|downshort_L_left|downshort_L_right|uptall_L_left|uptall_L_right|upshort_L_left|upshort_L_right) & (isWhite ? ~White:~Black); 

}


BitBoard find_rookmoves(BitBoard Piece, int isWhite) {

    BitBoard RankMask= findRank(Piece); //this works
    BitBoard o = RankMask & Main; //this works 
    BitBoard o_r = reverse(o); //this works
    BitBoard r = Piece;
    BitBoard r_r = reverse(Piece); // this works
    BitBoard FileMask = findFile(Piece);

    

    BitBoard left = o ^ ((o|FileH)-2*r); // this works but still have wall of 1's

    left = left & ~FileH;

    BitBoard right = o_r ^ (((o_r|FileH)-(2*r_r)));
    right = right & ~FileH;
    right = reverse(right);

    BitBoard file_attc = fileAttacks((Main&~Piece),FileMask,Piece);
    
    return ((file_attc^right^left) & (isWhite ? (~White) : (~Black))); 
    
}
BitBoard find_allrookmoves(BitBoard Pieces, int isWhite) {
    
    BitBoard tempmove = 0;
    BitBoard moves = 0;
        

    BitBoard Piece_LSB;
    BitBoard Temp = Pieces;

        int size = countSetBits(Pieces);
        
        if(size > 1) {
            for(int i = 0; i < size; i++) {
                //gets LSB to isolate the piece on the board
                Piece_LSB = getLSB(Temp);
                tempmove = find_rookmoves(Piece_LSB, isWhite);
                moves = tempmove | moves;
                //removes the LSB then get the next(gets moves for each rook on the board)
                Temp = removeLSB(Temp);

            }

        }else {
            
            
            BitBoard one = find_rookmoves(Pieces,isWhite);
            
            return one;
        }


        return moves;


    }
// #3 Bishop moves
BitBoard find_bishopmoves(BitBoard Piece, int isWhite) {

    BitBoard dia;
    BitBoard antiDia;

    dia = fileAttacks((Main& ~Piece),findDia(Piece),Piece);

    antiDia = fileAttacks((Main& ~Piece),findADia(Piece),Piece);

    return ((dia ^ antiDia) & (isWhite ? (~White): (~Black)));

}
BitBoard find_allbishopmoves(BitBoard Pieces, int isWhite) {

        BitBoard tempmove = 0;
        BitBoard moves = 0;
            

        BitBoard Piece_LSB;
        BitBoard Temp = Pieces;

        int size = countSetBits(Pieces);

        if(size > 1) {
            for(int i = 0; i < size; i++) {
                //gets LSB to isolate the piece on the board
                Piece_LSB = getLSB(Temp);
                tempmove = find_bishopmoves(Piece_LSB, isWhite);
                moves = tempmove | moves;
                //removes the LSB then get the next(gets moves for each bishop on the board)
                Temp = removeLSB(Temp);

            }

        }else {
                    
            BitBoard one = find_bishopmoves(Pieces,isWhite);
            
            return one;
        }

        return moves;



    }
// #2 Queen moves: uses bishop and rook moves combined
BitBoard find_queenmoves(BitBoard Piece, int isWhite) {
    return (find_allbishopmoves(Piece, isWhite) | find_allrookmoves(Piece, isWhite));
}

int castle(int isWhite) {


    if(isWhite) {
            if((WcastleR) || (WcastleL)) {
                
                if(((((Main) & (castleWR)) == 0) || (((Main) & (castleWL)) == 0))) {
                    if((((White_rooks) & (A1)) != 0) ||(((White_rooks) & (H1)) != 0) ) {
                        
                        return 1;
                    }
                }
            }
    }else {
            if((BcastleR) || (BcastleL)) {
                if(((((Main) & (castleBR)) == 0) || (((Main) & (castleBL)) == 0))) {
                    if((((Black_rooks) & (A8)) != 0) ||(((Black_rooks) & (H8)) != 0) ) {
                        return 1;
                    } 
                }
        }
    }
    
    return 0;
   
}



BitBoard vaildMovesWhite() {
    
    BitBoard pawns = find_pawnmoves(White_pawns,1);
    
    BitBoard rooks = find_allrookmoves(White_rooks,1);
    
    BitBoard knights = find_knightmoves(White_knights,1);
    
    BitBoard king = find_kingmoves(White_king,1);
    
    BitBoard queens = find_queenmoves(White_queen,1);
    BitBoard bishops = find_allbishopmoves(White_bishops,1);
 

    return (pawns|rooks|knights|king|queens|bishops);
    
}
BitBoard vaildMovesBlack() {

    BitBoard pawns = find_pawnmoves(Black_pawns,0);
    BitBoard rooks = find_allrookmoves(Black_rooks,0);
    BitBoard knights = find_knightmoves(Black_knights,0);
    BitBoard king = find_kingmoves(Black_king,0);
    BitBoard queens = find_queenmoves(Black_queen,0);
    BitBoard bishops = find_allbishopmoves(Black_bishops,0);

    return (pawns|rooks|knights|king|queens|bishops);
    
}
BitBoard all_VaildMoves() {
    
    return(vaildMovesWhite() | vaildMovesBlack());
}

MoveList find_allrooks(int isWhite) {
    MoveList moves;
    moves.size = 0;  // Initialize the size of the MoveList
     int num;
    num = isWhite  ? (countSetBits(White_rooks)) : (countSetBits(Black_rooks));
    if(num == 0) {
        return (MoveList){0};

    }
    //Move allmoves[32];
    Move* allmoves = (Move*)malloc(sizeof(Move)*(16*num));
    moves.moves = allmoves;

    
    int numofmoves;

    BitBoard rooks = isWhite ? White_rooks : Black_rooks;
    BitBoard temp = rooks;

    while (temp) {
        BitBoard rookSquare = getLSB(temp);
        temp = removeLSB(temp);

        // Get valid rook moves for the current rook
        BitBoard validMoves = find_rookmoves(rookSquare, isWhite);

        // Add each valid move to the moves array
        numofmoves = countSetBits(validMoves);
        
        if (numofmoves) {
            BitBoard vm = validMoves;
            while (vm) {
                BitBoard square = getLSB(vm);
                vm = removeLSB(vm);
                moves.moves[moves.size].Square = square;
                moves.moves[moves.size].Piece = rookSquare;
                moves.size++;
            }
        }
    }

    return moves;
}
MoveList find_allbishops(int isWhite) {
    MoveList moves;
    moves.size = 0;  // Initialize the size of the MoveList
    int num;
    num = isWhite  ? (countSetBits(White_bishops)) : (countSetBits(Black_bishops));
    if(num == 0) {
        return (MoveList){0};

    }

    //Move allmoves[16*num];
    Move *allmoves = (Move *)malloc(sizeof(Move) * (16*num));
    moves.moves = allmoves;

    
    int numofmoves;

    BitBoard bishops = isWhite ? White_bishops : Black_bishops;
    BitBoard temp = bishops;

    while (temp) {
        BitBoard bishopSquare = getLSB(temp);
        temp = removeLSB(temp);

        // Get valid rook moves for the current rook
        BitBoard validMoves = find_bishopmoves(bishopSquare, isWhite);

        // Add each valid move to the moves array
        numofmoves = countSetBits(validMoves);
        
        if (numofmoves) {
            BitBoard vm = validMoves;
            while (vm) {
                BitBoard square = getLSB(vm);
                vm = removeLSB(vm);
                moves.moves[moves.size].Square = square;
                moves.moves[moves.size].Piece = bishopSquare;
                moves.size++;
            }
        }
    }

    return moves;
}
MoveList find_allqueens(int isWhite) {
     MoveList moves;
    moves.size = 0;  // Initialize the size of the MoveList
    int numqueens;
    numqueens = isWhite  ? (countSetBits(White_queen)) : (countSetBits(Black_queen));
    if(numqueens == 0) {
        return (MoveList){0};

    }
    //Move allmoves[32*numqueens];
    Move * allmoves = (Move*)malloc(sizeof(Move) * (32*numqueens)); 
    moves.moves = allmoves;

    
    int numofmoves;

    BitBoard queens = isWhite ? White_queen : Black_queen;
    BitBoard temp = queens;

    while (temp) {
        BitBoard QueenSquare = getLSB(temp);
        temp = removeLSB(temp);

        // Get valid rook moves for the current rook
        BitBoard validMoves = find_bishopmoves(QueenSquare, isWhite) | find_rookmoves(QueenSquare,isWhite);

        // Add each valid move to the moves array
        numofmoves = countSetBits(validMoves);
        
        if (numofmoves) {
            BitBoard vm = validMoves;
            while (vm) {
                BitBoard square = getLSB(vm);
                vm = removeLSB(vm);
                moves.moves[moves.size].Square = square;
                moves.moves[moves.size].Piece = QueenSquare;
                moves.size++;
            }
        }
    }

    return moves;
}
MoveList find_allknights(int isWhite) {
    MoveList movesList;
    movesList.size = 0;

    int nKnights = countSetBits(isWhite ? White_knights : Black_knights);
    if (nKnights == 0) return (MoveList){0};

    // Up to 8 moves per knight (including captures)
    Move *moves = (Move *)malloc(sizeof(Move) * (nKnights * 8));
    if (!moves) return (MoveList){0};

    BitBoard temp = isWhite ? White_knights : Black_knights;
    while (temp) {
        BitBoard knightSquare = getLSB(temp);
        temp = removeLSB(temp);

        BitBoard v = find_knightmoves(knightSquare, isWhite);
        while (v) {
            BitBoard to = getLSB(v);
            v = removeLSB(v);
            // Defensive guard against theoretical overflow
            if (movesList.size >= nKnights * 8) break;
            moves[movesList.size].Piece = knightSquare;
            moves[movesList.size].Square = to;
            movesList.size++;
        }
    }

    movesList.moves = moves;
    return movesList;
}
MoveList find_allking(int isWhite) {
    BitBoard Piece = isWhite ? (White_king) : (Black_king);
    BitBoard valid = isWhite ? (~White) : (~Black);
// Removed the piece on the sides of the boards
    BitBoard start_CA = Piece & ClearFile_A;
    BitBoard start_CH  = Piece & ClearFile_H;
   
    //Move moves[8];
    Move * moves = malloc(sizeof(Move) *10);

    int index = 0;
    
    if(Piece == 0) {
        return (MoveList){0};
    }

    //Move up 
   if((Piece << 8) & valid) {
    moves[index].Piece = Piece;
    moves[index].Square = (Piece << 8);
    index++;
   } 

    //Move down 
    if((Piece >> 8) & valid) {
        moves[index].Piece = Piece;
        moves[index].Square = (Piece >> 8);
        index++;
   } 
    //Move right
    if((start_CA << 1) & valid) {
        moves[index].Piece = Piece;
        moves[index].Square = (start_CA << 1);
        index++;
   } 
    //Move left 
    
    if((start_CH >> 1) & valid) {
        moves[index].Piece = Piece;
        moves[index].Square = (start_CH >> 1);
        index++;
   } 

    //Move top left, right

     if((start_CH << 7) & valid) {
        moves[index].Piece = Piece;
        moves[index].Square = (start_CH << 7);
        index++;
   } 
     if((start_CA << 9) & valid) {
        moves[index].Piece = Piece;
        moves[index].Square = (start_CA << 9);
        index++;
   } 
    //Move bottom left, right
     if((start_CH >> 9) & valid) {
        moves[index].Piece = Piece;
        moves[index].Square = (start_CH >> 9);
        index++;
   } 
  
     if((start_CA >> 7) & valid) {
        moves[index].Piece = Piece;
        moves[index].Square = (start_CA >> 7);
        index++;
   } 

    
    MoveList move;
    move.moves = moves;
    move.size = index;
    //ADd in castle moves
    if(castle(isWhite)) {
   
      MoveList castles = find_Castles(isWhite);
      MoveList allmoves = combineMoveLists(move,castles);
      free(move.moves);
      free(castles.moves);
      return allmoves;
    }
    

    return move;
}
MoveList find_Castles(int isWhite) {
    Move tempmove;

    int size = countSetBits(isWhite ? White_rooks : Black_rooks);
    
    
    Move *moves = (Move *)malloc(sizeof(Move) * size);
    
    

    BitBoard Piece_LSB;
    BitBoard Temp = isWhite ? White_rooks : Black_rooks;

    int index = 0;  // Use a separate index variable

    for (int i = 0; i < size; i++) {
        Piece_LSB = getLSB(Temp);
        tempmove = castlemove(Piece_LSB,isWhite);
        if(tempmove.Square) {
            moves[index] = tempmove;
            index++;
        }
        //removes the LSB then get the next
        Temp = removeLSB(Temp);
    }

    MoveList movelist;
    movelist.moves = moves;
    movelist.size = index;
    return movelist;
}
MoveList find_allpawns(int isWhite) {

    Move tempmove;
    Move tempmove2;
    Move tempmove3;
    Move tempmove4;

    int size = countSetBits(isWhite ? White_pawns : Black_pawns);
    
    //Move moves[size*4];
    Move * moves = (Move*)malloc(sizeof(Move) * (size*4));
    
    if (moves == NULL) {
        fprintf(stderr, "ERROR: malloc failed in find_allpawns\n");
        return (MoveList){.moves = NULL, .size = 0};
    }

    BitBoard Piece_LSB;
    BitBoard Temp = isWhite ? White_pawns : Black_pawns;

    int index = 0;  // Use a separate index variable

    for (int i = 0; i < size; i++) {
        //gets LSB to isolate the piece on the board
        Piece_LSB = getLSB(Temp);
        tempmove = find_pawnmoveup(Piece_LSB, isWhite);
        tempmove2 = find_pawnmovedown(Piece_LSB, isWhite);
        tempmove3 = find_pawntakeright(Piece_LSB, isWhite);
        tempmove4 = find_pawntakeleft(Piece_LSB, isWhite);
        if (tempmove4.Square) {
            moves[index].Square = tempmove4.Square;
            moves[index].Piece = tempmove4.Piece;
            index++;
        }
        if (tempmove3.Square) {
            moves[index].Square = tempmove3.Square;
            moves[index].Piece = tempmove3.Piece;
            index++;
        }
        if (tempmove2.Square) {
            moves[index].Square = tempmove2.Square;
            moves[index].Piece = tempmove2.Piece;
            index++;
        }
        if (tempmove.Square) {
            moves[index].Square = tempmove.Square;
            moves[index].Piece = tempmove.Piece;
            index++;
        }
        //removes the LSB then get the next pawn
        Temp = removeLSB(Temp);
    }

    MoveList movelist;
    movelist.moves = moves;
    movelist.size = index;

    return movelist;
}

MoveList combineMoveLists(MoveList list1, MoveList list2) {
    MoveList combinedList;

    int size = list1.size + list2.size;
    
    // Handle empty case
    if (size == 0) {
        combinedList.moves = NULL;
        combinedList.size = 0;
        return combinedList;
    }
    
    Move *moves = malloc(sizeof(Move) * (size));
    // Allocate memory for the combined moves
    combinedList.moves = moves;
    //(Move *)malloc(sizeof(Move) * (list1.size + list2.size));
    if (combinedList.moves == NULL) {
        // Handle allocation failure
        fprintf(stderr, "ERROR: malloc failed in combineMoveLists for %d moves\n", size);
        combinedList.size = 0;
        return combinedList;
    }

    // Copy moves from the first list
    for (int i = 0; i < list1.size; ++i) {
        combinedList.moves[i] = list1.moves[i];
    }

    // Copy moves from the second list
    for (int i = 0; i < list2.size; ++i) {
        combinedList.moves[list1.size + i] = list2.moves[i];
    }

    // Set the size of the combined list
    combinedList.size = list1.size + list2.size;
    //free other lists movearrays
    
    //////////////////////THIS IS BREAKING EVERYTHING FOR SOMEREASON
    //free(list1.moves);
   // free(list2.moves);
    
    return combinedList;
}


MoveList findMoves(int isWhite) {

    MoveList moves;
    MoveList pawnmoves = find_allpawns(isWhite);
    
    
    MoveList rookmoves = find_allrooks(isWhite);
  
    MoveList queenmoves = find_allqueens(isWhite);

    MoveList bishopmoves = find_allbishops(isWhite);
   
    MoveList kingmoves = find_allking(isWhite);
 

    MoveList knightmoves = find_allknights(isWhite);
    

    //rooks,queens,pawns
    MoveList RookpawnQueen = combineMoveLists(combineMoveLists(pawnmoves,rookmoves),queenmoves);
    if (rookmoves.moves) free(rookmoves.moves);
    if (pawnmoves.moves) free(pawnmoves.moves);
    if (queenmoves.moves) free(queenmoves.moves);

    MoveList KingKnightBishop = combineMoveLists(combineMoveLists(kingmoves,knightmoves),bishopmoves);
    if (kingmoves.moves) free(kingmoves.moves);
    if (knightmoves.moves) free(knightmoves.moves);
    if (bishopmoves.moves) free(bishopmoves.moves);
   
    moves = combineMoveLists(RookpawnQueen,KingKnightBishop);
    if (RookpawnQueen.moves) free(RookpawnQueen.moves);
    if (KingKnightBishop.moves) free(KingKnightBishop.moves);
    
    

    MoveList validmoves = filterChecks(moves,isWhite); 
    free(moves.moves);
    
    return validmoves;
}


Move castlemove(BitBoard Piece,int isWhite) {
    Move move;
    
    //  FIX: Only allow castling if the rook is on its starting square
    // AND the correspawning castling right is still available
    if(isWhite) {
        // White queenside castle: rook must be on A1 and WcastleL must be true
        if((Piece == A1) && (WcastleL) && ((Main & castleWL) == 0)) {
            // King can't be in check, and squares E1, D1, C1 must not be attacked
            if(!is_square_attacked(E1, 0) && !is_square_attacked(D1, 0) && !is_square_attacked(C1, 0)) {
                move.Piece = E1;
                move.Square = C1;
                move.castle = 1;
                return move;
            }
        }
        // White kingside castle: rook must be on H1 and WcastleR must be true
        else if((Piece == H1) && (WcastleR) && ((Main & castleWR) == 0)) {
            // Squares E1, F1, G1 must not be attacked
            if(!is_square_attacked(E1, 0) && !is_square_attacked(F1, 0) && !is_square_attacked(G1, 0)) {
                move.Piece = E1;
                move.Square = G1;
                move.castle = 2;
                return move;
            }
        }
    } else {
        // Black queenside castle: rook must be on A8 and BcastleL must be true
        if((Piece == A8) && (BcastleL) && ((Main & castleBL) == 0)) {
            if(!is_square_attacked(E8, 1) && !is_square_attacked(D8, 1) && !is_square_attacked(C8, 1)) {
                move.Piece = E8;
                move.Square = C8;
                move.castle = 3;
                return move;
            }
        }
        // Black kingside castle: rook must be on H8 and BcastleR must be true
        else if((Piece == H8) && BcastleR && ((Main & castleBR) == 0)) {
            if(!is_square_attacked(E8, 1) && !is_square_attacked(F8, 1) && !is_square_attacked(G8, 1)) {
                move.Piece = E8;
                move.Square = G8;
                move.castle = 4;
                return move;
            }
        }
    }
    
    // No valid castle move
    return (Move){0};
}

int checkmate(int isWhite) { 
    MoveList valid = findMoves(isWhite);

    if(valid.size == 0) {

        if(isCheck(isWhite)) {
            
            return 2;
        }else {
            
            return 1;
        }
        }else{
            return 0;
    }

}


int isCheck(int isWhite) {

    // Use true attack maps (no castles, no pawn pushes)
    if (isWhite) {
        return (attacks_by_black() & White_king) ? 1 : 0;
    } else {
        return (attacks_by_white() & Black_king) ? 1 : 0;
    }
}

MoveList filterChecks(MoveList moves, int isWhite) {
    MoveList validmoves;
    
    // Check if memory allocation succeeds
    Move * move = (Move *)malloc(sizeof(Move) * moves.size);
    
    if (move == NULL) {
        fprintf(stderr, "ERROR: Memory allocation failed in filterChecks\n");
        return (MoveList){.moves = NULL, .size = 0};
    }

    int index = 0;

   
    
    for (int i = 0; i < moves.size; i++) {
    // Snapshot castling rights as they are mutated by move/undo
    int WcL = WcastleL, WcR = WcastleR, BcL = BcastleL, BcR = BcastleR;
        
    // Capture board state before simulation
    BoardState prevState;
    prevState.White_pawns = White_pawns;
    prevState.White_knights = White_knights;
    prevState.White_rooks = White_rooks;
    prevState.White_bishops = White_bishops;
    prevState.White_queen = White_queen;
    prevState.White_king = White_king;
    prevState.Black_pawns = Black_pawns;
    prevState.Black_knights = Black_knights;
    prevState.Black_rooks = Black_rooks;
    prevState.Black_bishops = Black_bishops;
    prevState.Black_queen = Black_queen;
    prevState.Black_king = Black_king;
        
        int savedCount = count;  // Save count before move_piece (which calls StoreMove)
        
        int ok = move_piece(moves.moves[i], isWhite);
        if (ok == 1) {
            // Check if the move doesn't result in a check
            if (!(isCheck(isWhite))) {
                move[index] = moves.moves[i];
                index++;
            }
            // CRITICAL FIX: Restore from saved state instead of unmakeMove()
            // unmakeMove() relies on MoveHistory[count] which gets corrupted by StoreMove calls
            White_pawns = prevState.White_pawns;
            White_knights = prevState.White_knights;
            White_rooks = prevState.White_rooks;
            White_bishops = prevState.White_bishops;
            White_queen = prevState.White_queen;
            White_king = prevState.White_king;
            Black_pawns = prevState.Black_pawns;
            Black_knights = prevState.Black_knights;
            Black_rooks = prevState.Black_rooks;
            Black_bishops = prevState.Black_bishops;
            Black_queen = prevState.Black_queen;
            Black_king = prevState.Black_king;
            updateAll();
        }
        // restore castling rights and move history count regardless
        WcastleL = WcL; WcastleR = WcR; BcastleL = BcL; BcastleR = BcR;
        count = savedCount;
    }

    validmoves.moves = move;
    validmoves.size = index;
   
    
    // Free memory if no valid moves found
    if (index == 0) {
        free(validmoves.moves);  // Free the allocated memory
        validmoves.moves = NULL;  // Set to NULL to prevent dangling pointer
        validmoves.size = 0;
        return validmoves;
    }

    return validmoves;
}


int take(BitBoard sqaure, int isWhite) {
    int type = find_type(sqaure);
   
    if(isWhite) {
        //for White Piece being taken
        switch (type)
        {
            case Typepawn:
                White_pawns = (White_pawns & ~sqaure);
                break;
            case Typeknight:
                White_knights = (White_knights & ~sqaure);
                break;
            case Typebishop:
                White_bishops = (White_bishops & ~sqaure);
                break;
            case Typerook:
                White_rooks = (White_rooks & ~sqaure);
                // If a rook is captured on its starting square, disable that side's castling
                if(sqaure == H1) WcastleR = 0;
                if(sqaure == A1) WcastleL = 0;
                break;
            case Typequeen:
                White_queen = (White_queen & ~sqaure);
                break;
            case Typeking: 
               // This should never happen in a valid game (should be handled by checkmate and endgame before) 
                break;
            default:
                break;
        }
    }else {
        //for Black Piece being taken
        switch (type)
        {
            case Typepawn:
                Black_pawns = (Black_pawns & ~sqaure);
                break;
            case Typeknight:
                Black_knights = (Black_knights & ~sqaure);
                break;
            case Typebishop:
                Black_bishops = (Black_bishops & ~sqaure);
                break;
            case Typerook:
                Black_rooks = (Black_rooks & ~sqaure);
                // If a rook is captured on its starting square, disable that side's castling
                if(sqaure == H8) BcastleR = 0;
                if(sqaure == A8) BcastleL = 0;
                break;
            case Typequeen:
                Black_queen = (Black_queen & ~sqaure);
                break;
            case Typeking: 
               // This should never happen in a valid game (should be handled by checkmate and endgame before) 
                break;
            default:
                break;
                
        }
    }
    updateAll();
    return type;
}

void StoreMove(Move move){
        //Note: prevState was already captured BEFORE the move was applied
        
        //updates the move to have the right black and white board note that the moved piece has been moved 
        updateAll();
        //move.Bblack = Black;
        //move.Bwhite = White;

    // Increase move count only if there is capacity; prevents out-of-bounds
    if (count + 1 >= MAX_MOVES) {
        // History at capacity: skip storing to prevent overflow
        // Search uses manual state restore, so this is safe.
        return;
    }
    count++;
    MoveHistory[count].move = move;
    MoveHistory[count].prevState.White_pawns = White_pawns;
    MoveHistory[count].prevState.White_knights = White_knights;
    MoveHistory[count].prevState.White_rooks = White_rooks;
    MoveHistory[count].prevState.White_bishops = White_bishops;
    MoveHistory[count].prevState.White_queen = White_queen;
    MoveHistory[count].prevState.White_king = White_king;
    MoveHistory[count].prevState.Black_pawns = Black_pawns;
    MoveHistory[count].prevState.Black_knights = Black_knights;
    MoveHistory[count].prevState.Black_rooks = Black_rooks;
    MoveHistory[count].prevState.Black_bishops = Black_bishops;
    MoveHistory[count].prevState.Black_queen = Black_queen;
    MoveHistory[count].prevState.Black_king = Black_king;
        
        

}

// Main function to move a piece 
int move_piece(Move move, int isWhite) {
    BitBoard Piece = move.Piece;
    BitBoard sqaure = move.Square;
    //printf("Moving piece from %llx to %llx\n", Piece, sqaure);
    int type = find_type(Piece);
    
    if(type == 0) {
        // Piece not on board - move is invalid
        return -1;
    }
 
 
    //Castling
    int c = 0;
    // Castling detection: king moved two squares horizontally (avoid global all_VaildMoves())
    // Need to validate that the move the valid
    if(type == Typeking) {
        // if castling (i.e king moves two squares)
        if((Piece >> 2) == sqaure) {
            if (isWhite)
            {
                //Check castling rights
                if(!WcastleR) {
                    return -1;
                }
            }else 
            {
                if(!BcastleR) {
                    return -1;
                }
            }
            c = isWhite ? 2 : 4; // king side
        } else if((Piece << 2) == sqaure) {
            if(isWhite) {
                if(!WcastleL) {
                    return -1;
                }
            } else {
                if(!BcastleL) {
                    return -1;
                }
            }
            c = isWhite ? 1 : 3; // queen side
        }
    }
    
    if(c == 1 || c == 2 || c ==3 || c ==4) {
        switch (c)
        {
        case 1:
            //A file White
            White_rooks = (White_rooks & ~A1) | D1;
            White_king = C1;
            //updateAll();
            StoreMove(move);
            WcastleL = 0; WcastleR = 0; // castling spends both rights
            return 1;
            break;
        case 2:
            //H file White
            White_rooks = (White_rooks & ~H1) | F1;
            White_king = G1;
            //updateAll();
            StoreMove(move);
            WcastleL = 0; WcastleR = 0; // castling spends both rights
            return 1;
            break;
        case 3:
            //A file Black
            Black_rooks = (Black_rooks & ~A8) | D8;
            Black_king = C8;
            //updateAll();
            StoreMove(move);
            BcastleL = 0; BcastleR = 0; // castling spends both rights
            return 1;
            break;
        case 4:
            //H file Black
            Black_rooks = (Black_rooks & ~H8) | F8;
            Black_king = G8;
            //updateAll();
            StoreMove(move);
            BcastleL = 0; BcastleR = 0; // castling spends both rights
            return 1;
            break;
        default:
            break;
        }
    }




    BitBoard pawnMoves = find_pawnmoves(Piece, isWhite);
////For Promotion of a pawn, now handles capture promotions correctly (only to queen for now)
    if(type == Typepawn) {
        //Check that the move is valid before promoting
        // Just check if pawn is on second last rank and moving to last rank
        if((sqaure & pawnMoves & Rank1) || (sqaure &  pawnMoves & Rank8)) {
            if(isWhite) {
                // If capturing on the last rank, remove the enemy piece first
                if (sqaure & Black) {
                    move.capturetype = take(sqaure, 0); // captured a black piece
                }
                White_pawns = White_pawns & ~Piece;
                White_queen = White_queen | sqaure;
                StoreMove(move);
                return 1;
            } else {
                if (sqaure & White) {
                    move.capturetype = take(sqaure, 1); // captured a white piece
                }
                Black_pawns = Black_pawns & ~Piece;
                Black_queen = Black_queen | sqaure;
                StoreMove(move);
                return 1;
            }
        }
    }

 
    // Removed expensive global all_VaildMoves() presence test; each piece case validates
    // the move with its own generator. Keeping the global union was a major bottleneck.
   
    move.capturetype = 0;

    if(isWhite) {
        switch (type)
        {
        case Typepawn:
            if(sqaure & pawnMoves) {
                if(sqaure &(isWhite ? Black:White)) {
                    //this means it capturing a piece and must update both boards
                    move.capturetype = take(sqaure, 0); //function to get rid of peice on that bitboard, and update the score
                    White_pawns = (White_pawns & ~Piece) | sqaure;
                    //updateAll();
                    StoreMove(move);
                    return 1;
                }else {
                    //not capturing
                    White_pawns = (White_pawns & ~Piece) | sqaure;
                    StoreMove(move);
                    //updateAll();
                    
                    return 1;
                    
                }
            } 
            break;
        case Typeknight: 
        //knights
            if(sqaure & find_knightmoves(Piece, isWhite)) {
                if(sqaure &(isWhite ? Black:White)) {
                    //this means it capturing a piece and must update both boards
                    move.capturetype = take(sqaure, 0); //function to get rid of peice on that bitboard, and update the score
                    White_knights = (White_knights & ~Piece) | sqaure;
                    //updateAll();
                    StoreMove(move);
                    return 1;

                }else {
                    //not capturing
                    White_knights = (White_knights & ~Piece) | sqaure;
                    //updateAll();
                    StoreMove(move);
                    return 1;
                    
                }
            } 
            break;
        case Typebishop:
        //bishops
            if(sqaure & find_bishopmoves(Piece, isWhite)) {
                if(sqaure &(isWhite ? Black:White)) {
                    //this means it capturing a piece and must update both boards
                    move.capturetype = take(sqaure, 0); //function to get rid of peice on that bitboard, and update the score
                    White_bishops = (White_bishops & ~Piece) | sqaure;
                    StoreMove(move);
                    return 1;

                }else {
                    //not capturing
                    White_bishops = (White_bishops & ~Piece) | sqaure;
                    //updateAll();
                    StoreMove(move);
                    return 1;
                    
                }
            } 
            break;
        case Typerook:
        //rooks
            if(sqaure & find_rookmoves(Piece, isWhite)) {
                if(sqaure &(isWhite ? Black:White)) {
                    //this means it capturing a piece and must update both boards
                    move.capturetype = take(sqaure, 0); //function to get rid of peice on that bitboard, and update the score
                    White_rooks = (White_rooks & ~Piece) | sqaure;
                    //updateAll();
                    StoreMove(move);
                    // Only disable castling if rook moves FROM its starting square
                    if(Piece == H1) {
                        WcastleR = 0;
                    }
                    if(Piece == A1) {
                        WcastleL = 0;
                    }
                    return 1;

                }else {
                    //not capturing
                    White_rooks = (White_rooks & ~Piece) | sqaure;
                    //updateAll();
                    StoreMove(move);
                    // Only disable castling if rook moves FROM its starting square
                    if(Piece == H1) {
                        WcastleR = 0;
                    }
                    if(Piece == A1) {
                        WcastleL = 0;
                    }
                    return 1;
                }
            } 
            break;
        case Typequeen:
        //queen
            if(sqaure & find_queenmoves(Piece, isWhite)) {
                if(sqaure &(isWhite ? Black:White)) {
                    //this means it capturing a piece and must update both boards
                    move.capturetype = take(sqaure, 0); //function to get rid of peice on that bitboard, and update the score
                    White_queen = (White_queen & ~Piece) | sqaure;
                   // updateAll();
                    StoreMove(move);
                    return 1;
                }else {
                    //not capturing
                    White_queen = (White_queen & ~Piece) | sqaure;
                    //updateAll();
                    StoreMove(move);
                    return 1;
                }
            } 
            break;
        case Typeking:
        //king
            
            if(sqaure & find_kingmoves(Piece, isWhite)) {
                if(sqaure &(isWhite ? Black:White)) {
                    //this means it capturing a piece and must update both boards
                    move.capturetype = take(sqaure, 0); //function to get rid of peice on that bitboard, and update the score
                    White_king = (White_king & ~Piece) | sqaure;
                    //updateAll();
                    //printf("\nMAde it here\n\n");
                    StoreMove(move);
                    WcastleL = 0;
                    WcastleR = 0;
                    //printf("MAde it here");
                    return 1;
                }else {
                    //not capturing
                    White_king = (White_king & ~Piece) | sqaure;
                   // updateAll();
                    StoreMove(move);
                    WcastleL = 0;
                    WcastleR = 0;
                    return 1;
                }
            }
            break; 
        default:
            printf("Invaild Move");
            return -1;
            break;
        } 
    }else {
        switch (type)
        {
        case Typepawn:
            if(sqaure & find_pawnmoves(Piece, isWhite)) {
                if(sqaure &(isWhite ? Black:White)) {
                    //this means it capturing a piece and must update both boards
                    move.capturetype = take(sqaure, 1); //function to get rid of peice on that bitboard, and update the score
                    Black_pawns = (Black_pawns & ~Piece) | sqaure;
                   // updateAll();
                    StoreMove(move);
                    return 1;
                }else {
                    //not capturing
                    Black_pawns = (Black_pawns & ~Piece) | sqaure;
                    //updateAll();
                    StoreMove(move);
                    return 1;
                }
            } 
            break;
        case Typeknight: 
        //knights
            if(sqaure & find_knightmoves(Piece, isWhite)) {
                if(sqaure &(isWhite ? Black:White)) {
                    //this means it capturing a piece and must update both boards
                    move.capturetype = take(sqaure, 1); //function to get rid of peice on that bitboard, and update the score
                    Black_knights = (Black_knights & ~Piece) | sqaure;
                   // updateAll();
                    StoreMove(move);
                    return 1;
                }else {
                    //not capturing
                    Black_knights = (Black_knights & ~Piece) | sqaure;
                   // updateAll();
                    StoreMove(move);
                    return 1;
                }
            } 
            break;
        case Typebishop:
        //bishops
            if(sqaure & find_bishopmoves(Piece, isWhite)) {
                if(sqaure &(isWhite ? Black:White)) {
                    //this means it capturing a piece and must update both boards
                    move.capturetype = take(sqaure, 1); //function to get rid of peice on that bitboard, and update the score
                    Black_bishops = (Black_bishops & ~Piece) | sqaure;
                    //updateAll();
                    StoreMove(move);
                    return 1;
                }else {
                    //not capturing
                    Black_bishops = (Black_bishops & ~Piece) | sqaure;
                   // updateAll();
                    StoreMove(move);
                    return 1;
                }
            } 
            break;
        case Typerook:
        //rooks
            if(sqaure & find_rookmoves(Piece, isWhite)) {
                if(sqaure &(isWhite ? Black:White)) {
                    //this means it capturing a piece and must update both boards
                    move.capturetype = take(sqaure, 1); //function to get rid of peice on that bitboard, and update the score
                    Black_rooks = (Black_rooks & ~Piece) | sqaure;
                    //updateAll();
                    StoreMove(move);
                    // Only disable castling if rook moves FROM its starting square
                    if(Piece == H8) {
                        BcastleR = 0;
                    }
                    if(Piece == A8) {
                        BcastleL = 0;
                    }
                    return 1;
                }else {
                    //not capturing
                    Black_rooks = (Black_rooks & ~Piece) | sqaure;
                   // updateAll();
                    StoreMove(move);
                    // Only disable castling if rook moves FROM its starting square
                    if(Piece == H8) {
                        BcastleR = 0;
                    }
                    if(Piece == A8) {
                        BcastleL = 0;
                    }
                    return 1;
                }
            } 
            break;
        case Typequeen:
        //queen
            if(sqaure & find_queenmoves(Piece, isWhite)) {
                if(sqaure &(isWhite ? Black:White)) {
                    //this means it capturing a piece and must update both boards
                    move.capturetype = take(sqaure,1); //function to get rid of peice on that bitboard, and update the score
                    Black_queen = (Black_queen & ~Piece) | sqaure;
                    //updateAll();
                    StoreMove(move);
                    return 1;
                }else {
                    //not capturing
                    Black_queen = (Black_queen & ~Piece) | sqaure;
                   // updateAll();
                    StoreMove(move);
                    return 1;
                }
            } 
            break;
        case Typeking:
        //king
            if(sqaure & find_kingmoves(Piece, isWhite)) {
                if(sqaure &(isWhite ? Black:White)) {
                    //this means it capturing a piece and must update both boards
                    move.capturetype = take(sqaure, 1); //function to get rid of peice on that bitboard, and update the score
                    Black_king = (Black_king & ~Piece) | sqaure;
                   // updateAll();
                    StoreMove(move);
                    BcastleL = 0;
                    BcastleR = 0;
                    return 1;
                }else {
                    //not capturing
                    Black_king = (Black_king & ~Piece) | sqaure;
                  //  updateAll();
                    StoreMove(move);
                    BcastleL = 0;
                    BcastleR = 0;
                    return 1;
                }
            }
            break; 
        default:
            printf("Invaild Move");
            return -1;
            break;
        } 
    }

    return -1;
}

// This function attempts to make a move
// Used for player moves and for AI moves
// Calls move_piece() which handles actual piece movement and captures, and also stores the move in history
// Could make this faster for AI by skipping validation as AI only looks at valid moves
int makeMove(Move move, int isWhite) {
    // Capture board state BEFORE making the move
    BoardState savedState;
    savedState.White_pawns = White_pawns;
    savedState.White_knights = White_knights;
    savedState.White_rooks = White_rooks;
    savedState.White_bishops = White_bishops;
    savedState.White_queen = White_queen;
    savedState.White_king = White_king;
    savedState.Black_pawns = Black_pawns;
    savedState.Black_knights = Black_knights;
    savedState.Black_rooks = Black_rooks;
    savedState.Black_bishops = Black_bishops;
    savedState.Black_queen = Black_queen;
    savedState.Black_king = Black_king;

    // Also snapshot castling rights and history count to fully restore on invalid moves
    int savedWcL = WcastleL, savedWcR = WcastleR, savedBcL = BcastleL, savedBcR = BcastleR;
    int savedCount = count;

    if(move_piece(move,isWhite) == -1) {
        printf("Failed to move piece\n");
        return 0;
    }

    // If move was stored in history by move_piece, attach the pre-move snapshot
    // so unmakeMove has the correct previous state.
    if (count > savedCount) {
        MoveHistory[count].prevState = savedState;
    }

    // Check if the move leaves the player in check
    if(isCheck(isWhite)) {
        //printf("Invalid move: %s would be in check!\n", isWhite ? "White" : "Black");
        // Undo the move
        // Since the move has not been pushed on the stack yet, must restore the board state manually
        White_pawns = savedState.White_pawns;
        White_knights = savedState.White_knights;
        White_rooks = savedState.White_rooks;
        White_bishops = savedState.White_bishops;
        White_queen = savedState.White_queen;
        White_king = savedState.White_king;
        Black_pawns = savedState.Black_pawns;
        Black_knights = savedState.Black_knights;
        Black_rooks = savedState.Black_rooks;
        Black_bishops = savedState.Black_bishops;
        Black_queen = savedState.Black_queen;
        Black_king = savedState.Black_king;
        // Restore castling rights and move history count that may have changed
        WcastleL = savedWcL; WcastleR = savedWcR; BcastleL = savedBcL; BcastleR = savedBcR;
        count = savedCount;
        updateAll();
        return 0;
    }

    return 1;

    
}

void undocapture(Move made, int captured) {
//if captured currecly unmake black pieces so that means black moved last and took a white piece
    if(captured) {
        //means white was captured 
        switch (made.capturetype)
        {
            case Typepawn:
                White_pawns = White_pawns | made.Square;
                break;
            case Typeknight:
                White_knights = White_knights | made.Square;
                break;
            case Typebishop:
                White_bishops = White_bishops | made.Square;
                break;
            case Typerook:
                White_rooks = White_rooks | made.Square;
                break;
            case Typequeen: 
                White_queen = White_queen | made.Square;
                break;
            case Typeking:
                White_king = White_king | made.Square;
                break;
            default:
                break;

        }
    }else {
        switch (made.capturetype)
        {
            case Typepawn:
                Black_pawns = Black_pawns | made.Square;
                break;
            case Typeknight:
                Black_knights = Black_knights | made.Square;
                break;
            case Typebishop:
                Black_bishops = Black_bishops | made.Square;
                break;
            case Typerook:
                Black_rooks = Black_rooks | made.Square;
                break;
            case Typequeen: 
                Black_queen = Black_queen | made.Square;
                break;
            case Typeking:
                Black_king = Black_king | made.Square;
                break;
            default:
                break;

        }
        

    }

}

// #5 Evaluation functions
static inline int eval_piece_positions(BitBoard pieces, const int* posTable) {
    int score = 0;
    BitBoard temp = pieces;
    
    while (temp) {
        BitBoard piece_bb = temp & -temp;  // Extract LSB
        int square = __builtin_ctzll(piece_bb);  // Get bit position (0-63)
        score += posTable[square];
        temp &= temp - 1;  // Clear LSB
    }


    return score;
}

static inline int pawn_eval(BitBoard White, BitBoard Black)
{
    // Check for double pawns, isolated pawns, and passed pawns (combined loop for performance)
    int score = 0;
    
    // Single loop to check all pawn structure features per file
    for(int file = 0; file < 8; file++) {
        BitBoard fileMask = FileA >> file;
        BitBoard whitePawnsInFile = White_pawns & fileMask;
        BitBoard blackPawnsInFile = Black_pawns & fileMask;

        int whitePawnCount = countSetBits(whitePawnsInFile);
        int blackPawnCount = countSetBits(blackPawnsInFile);

        // 1. Double pawns - penalize having multiple pawns on same file
        if(whitePawnCount > 1) {
            score -= (whitePawnCount - 1) * 20; // White penalty hurts White
        }
        if(blackPawnCount > 1) {
            score += (blackPawnCount - 1) * 20; // Black penalty helps White (FIX: was subtracting)
        }

        // 2. Isolated pawns - penalize pawns with no friendly pawns on adjacent files
        if(whitePawnsInFile) {
            BitBoard adjacentFiles = 0;
            if(file > 0) adjacentFiles |= FileA >> (file - 1);  // FIX: shift right
            if(file < 7) adjacentFiles |= FileA >> (file + 1);  // FIX: shift right
            if(!(White_pawns & adjacentFiles)) {
                score -= 15; // White isolated pawn penalty
            }
        }
        if(blackPawnsInFile) {
            BitBoard adjacentFiles = 0;
            if(file > 0) adjacentFiles |= FileA >> (file - 1);  // FIX: shift right
            if(file < 7) adjacentFiles |= FileA >> (file + 1);  // FIX: shift right
            if(!(Black_pawns & adjacentFiles)) {
                score += 15; // Black isolated pawn helps White (FIX: was subtracting)
            }
        }

        // 3. Passed pawns - bonus for pawns with no enemy pawns blocking/attacking ahead
        if(whitePawnsInFile) {
            BitBoard aheadAndAdjacent = 0;
            for(int f = (file > 0 ? file - 1 : file); f <= (file < 7 ? file + 1 : file); f++) {
                // FIX: White pawns move UP (toward rank 8), exclude Rank1-2, only check ahead
                aheadAndAdjacent |= (FileA >> f) & (Rank3 | Rank4 | Rank5 | Rank6 | Rank7 | Rank8);
            }
            if(!(Black_pawns & aheadAndAdjacent)) {
                score += 30; // White passed pawn bonus
            }
        }
        if(blackPawnsInFile) {
            BitBoard aheadAndAdjacent = 0;
            for(int f = (file > 0 ? file - 1 : file); f <= (file < 7 ? file + 1 : file); f++) {
                // FIX: Black pawns move DOWN (toward rank 1), exclude Rank7-8, only check ahead
                aheadAndAdjacent |= (FileA >> f) & (Rank1 | Rank2 | Rank3 | Rank4 | Rank5 | Rank6);
            }
            if(!(White_pawns & aheadAndAdjacent)) {
                score -= 30; // Black passed pawn helps Black (FIX: was adding to score)
            }
        }
    }

    return score;
}


// Material + positional evaluation from White's perspective (positive favors White)
int eval_position() {
    // DO NOT check for checkmate here - it's way too expensive!
    // Checkmate detection is handled in minimax/quiesce when moves.size == 0
    // This function is called millions of times and must be FAST
    
    int WhiteScore = 0;
    int BlackScore = 0;
    
    // Material evaluation
    int numberOfpawnsWhite = countSetBits(White_pawns);
    int numberOfpawnsBlack = countSetBits(Black_pawns);
    int numberOfKnightsWhite = countSetBits(White_knights);
    int numberOfKnightsBlack = countSetBits(Black_knights);
    int numberOfRooksWhite = countSetBits(White_rooks);
    int numberOfRooksBlack = countSetBits(Black_rooks);
    int numberOfQueensWhite = countSetBits(White_queen);
    int numberOfQueensBlack = countSetBits(Black_queen);
    int numberOfBishopsWhite = countSetBits(White_bishops);
    int numberOfBishopsBlack = countSetBits(Black_bishops);

    WhiteScore += (numberOfpawnsWhite * pawnValue);    
    WhiteScore += (numberOfKnightsWhite * KnightValue);    
    WhiteScore += (numberOfBishopsWhite * BishopValue);   
    WhiteScore += (numberOfRooksWhite * RookValue);      
    WhiteScore += (numberOfQueensWhite * QueenValue);    
    
    BlackScore += (numberOfpawnsBlack * pawnValue);
    BlackScore += (numberOfKnightsBlack * KnightValue);
    BlackScore += (numberOfBishopsBlack * BishopValue);
    BlackScore += (numberOfRooksBlack * RookValue);
    BlackScore += (numberOfQueensBlack * QueenValue);

    // Positional evaluation
    if (White_pawns) WhiteScore += eval_piece_positions(White_pawns, WHITE_PAWN_POSITION_TABLE);
    if (Black_pawns) BlackScore += eval_piece_positions(Black_pawns, BLACK_PAWN_POSITION_TABLE);
    
    if (White_knights) WhiteScore += eval_piece_positions(White_knights, WHITE_KNIGHT_POSITION_TABLE);
    if (Black_knights) BlackScore += eval_piece_positions(Black_knights, BLACK_KNIGHT_POSITION_TABLE);
    
    if (White_bishops) WhiteScore += eval_piece_positions(White_bishops, WHITE_BISHOP_POSITION_TABLE);
    if (Black_bishops) BlackScore += eval_piece_positions(Black_bishops, BLACK_BISHOP_POSITION_TABLE);
    
    if (White_rooks) WhiteScore += eval_piece_positions(White_rooks, WHITE_ROOK_POSITION_TABLE);
    if (Black_rooks) BlackScore += eval_piece_positions(Black_rooks, BLACK_ROOK_POSITION_TABLE);
    
    if (White_queen) WhiteScore += eval_piece_positions(White_queen, WHITE_QUEEN_POSITION_TABLE);
    if (Black_queen) BlackScore += eval_piece_positions(Black_queen, BLACK_QUEEN_POSITION_TABLE);
    
    if (White_king) WhiteScore += eval_piece_positions(White_king, WHITE_KING_POSITION_TABLE);
    if (Black_king) BlackScore += eval_piece_positions(Black_king, BLACK_KING_POSITION_TABLE);

    // King Safety: Penalize if king is in check (high priority)
    if (isCheck(1)) {  // White king in check
        WhiteScore -= 50;
    }
    if (isCheck(0)) {  // Black king in check
        BlackScore -= 50;
    }
    
    // Castling bonus: Reward having castled (king on castled square + rook moved from corner)
    // White kingside castle: King on G1, rook NOT on H1
    if ((White_king & G1) && !(White_rooks & H1) && (White_rooks & (F1 | E1 | D1))) {
        WhiteScore += 40; // Significant bonus for kingside castle
    }
    // White queenside castle: King on C1, rook NOT on A1
    if ((White_king & C1) && !(White_rooks & A1) && (White_rooks & (D1 | E1 | F1))) {
        WhiteScore += 40; // Significant bonus for queenside castle
    }
    // Black kingside castle: King on G8, rook NOT on H8
    if ((Black_king & G8) && !(Black_rooks & H8) && (Black_rooks & (F8 | E8 | D8))) {
        BlackScore += 40; // Significant bonus for kingside castle
    }
    // Black queenside castle: King on C8, rook NOT on A8
    if ((Black_king & C8) && !(Black_rooks & A8) && (Black_rooks & (D8 | E8 | F8))) {
        BlackScore += 40; // Significant bonus for queenside castle
    }
    
    // Returns - if black is winning + if white is winning on pawn structure
    WhiteScore += pawn_eval(White_pawns, Black_pawns);

    return (WhiteScore - BlackScore);
}
    

// Old user move function
int userMove(int isWhite) {

    //take in user input, check if its right then make that move, check if vaild,
    char fileMove;
    int RankMove;
    char fileto;
    int rankto;
    Move user;

    while(1) {
        printf("\n%s's turn. Enter move FROM file (A-H): ", isWhite ? "White" : "Black");
        scanf(" %c",&fileMove);
        printf("Enter FROM rank (1-8): ");
        scanf("%d",&RankMove); 

        user.Piece = 1ULL << ((RankMove - 1) * 8 + (fileMove - 'A'));
        
        printf("Enter move TO file (A-H): ");
        scanf(" %c",&fileto);
        printf("Enter TO rank (1-8): ");
        scanf("%d",&rankto); 
        user.Square = 1ULL << ((rankto - 1) * 8 + (fileto- 'A'));

        
        int valid = makeMove(user,isWhite);
        printf("made MOVE!!!!\n\n");
        if (valid == 1) {
            return 1;
        } else {
            printf("\n*** Invalid move! Try again. ***\n");
        }
    }

}




// Quiescence search: extend search on noisy (capture) positions to avoid horizon effect
// mirror main minimax: alpha is best score for White (maximizing), beta is
// best score for Black (minimizing). Returns a score.
int quiesce(int alpha, int beta, int isMaximizingPlayer, int qdepth) {
    // Stand pat evaluation
    int standPat = eval_position();

    if (isMaximizingPlayer) {
        if (standPat > alpha) alpha = standPat;
        if (alpha >= beta) return beta; // Fail-high cutoff
    } else {
        if (standPat < beta) beta = standPat;
        if (beta <= alpha) return alpha; // Fail-low cutoff
    }

    // Depth guard to cap explosion
    if (qdepth >= QMAX_DEPTH) {
        return isMaximizingPlayer ? alpha : beta;
    }

    // Generate moves and filter to captures only
    MoveList moves = findMoves(isMaximizingPlayer);
    if (moves.moves == NULL || moves.size == 0) {
        // No moves; just return stand pat bounds
        return isMaximizingPlayer ? alpha : beta;
    }

    uint64_t enemyPieces = isMaximizingPlayer ? Black : White;

    // Simple capture extraction + MVV-LVA ordering (victim value descending)
    // Count captures
    int capCount = 0;
    for (int i = 0; i < moves.size; ++i) {
        if (moves.moves[i].Square & enemyPieces) capCount++;
    }
    if (capCount == 0) {
        if (moves.moves) free(moves.moves);
        return isMaximizingPlayer ? alpha : beta; // Quiet position
    }

    // In-place partition captures at front
    int writeIdx = 0;
    for (int i = 0; i < moves.size; ++i) {
        if (moves.moves[i].Square & enemyPieces) {
            if (i != writeIdx) {
                Move tmp = moves.moves[writeIdx];
                moves.moves[writeIdx] = moves.moves[i];
                moves.moves[i] = tmp;
            }
            writeIdx++;
        }
    }
    // Sort captures (0..capCount-1) by victim value descending (MVV). Attacker value omitted for simplicity.
    for (int i = 0; i < capCount - 1; ++i) {
        for (int j = i + 1; j < capCount; ++j) {
            int victimI = piece_value_from_type(find_type(moves.moves[i].Square));
            int victimJ = piece_value_from_type(find_type(moves.moves[j].Square));
            if (victimJ > victimI) {
                Move tmp = moves.moves[i];
                moves.moves[i] = moves.moves[j];
                moves.moves[j] = tmp;
            }
        }
    }

    // Search captures
    for (int i = 0; i < capCount; ++i) {
        Move move = moves.moves[i];
        // Save state (same as main search)
        BoardState savedState;
        savedState.White_pawns = White_pawns;
        savedState.White_knights = White_knights;
        savedState.White_rooks = White_rooks;
        savedState.White_bishops = White_bishops;
        savedState.White_queen = White_queen;
        savedState.White_king = White_king;
        savedState.Black_pawns = Black_pawns;
        savedState.Black_knights = Black_knights;
        savedState.Black_rooks = Black_rooks;
        savedState.Black_bishops = Black_bishops;
        savedState.Black_queen = Black_queen;
        savedState.Black_king = Black_king;
        int WcL = WcastleL, WcR = WcastleR, BcL = BcastleL, BcR = BcastleR;
        int savedCount = count;

        int made = makeMove(move, isMaximizingPlayer);
        if (!made) {
            // Restore and skip
            White_pawns = savedState.White_pawns; White_knights = savedState.White_knights; White_rooks = savedState.White_rooks; White_bishops = savedState.White_bishops; White_queen = savedState.White_queen; White_king = savedState.White_king;
            Black_pawns = savedState.Black_pawns; Black_knights = savedState.Black_knights; Black_rooks = savedState.Black_rooks; Black_bishops = savedState.Black_bishops; Black_queen = savedState.Black_queen; Black_king = savedState.Black_king;
            updateAll();
            WcastleL = WcL; WcastleR = WcR; BcastleL = BcL; BcastleR = BcR; count = savedCount;
            continue;
        }

        int score = quiesce(alpha, beta, !isMaximizingPlayer, qdepth + 1);

        // Restore
        White_pawns = savedState.White_pawns; White_knights = savedState.White_knights; White_rooks = savedState.White_rooks; White_bishops = savedState.White_bishops; White_queen = savedState.White_queen; White_king = savedState.White_king;
        Black_pawns = savedState.Black_pawns; Black_knights = savedState.Black_knights; Black_rooks = savedState.Black_rooks; Black_bishops = savedState.Black_bishops; Black_queen = savedState.Black_queen; Black_king = savedState.Black_king;
        updateAll();
        WcastleL = WcL; WcastleR = WcR; BcastleL = BcL; BcastleR = BcR; count = savedCount;

        if (isMaximizingPlayer) {
            if (score > alpha) alpha = score;
        } else {
            if (score < beta) beta = score;
        }
        if (beta <= alpha) break; // prune
    }

    if (moves.moves) free(moves.moves);
    return isMaximizingPlayer ? alpha : beta;
}

Move minimax(int depth, int alpha, int beta, int isMaximizingPlayer) {
    Move bestMove = (Move){0};
    int bestScore = (isMaximizingPlayer) ? NINF : INF;
    int foundValidMove = 0;  


    // 50M is a reasonable upper bound to prevent infinite loops from bugs
    if (movesEvaluated > 50000000) {
        fprintf(stderr, "WARNING: Search exceeded 50M nodes, aborting to prevent crash\n");
        Move emergency = (Move){0};
        emergency.score = eval_position();
        return emergency;
    }

    if (depth == 0) {
        Move leaf = (Move){0};
        int qscore = quiesce(alpha, beta, isMaximizingPlayer, 0);
        leaf.score = qscore;
        return leaf;
    }

    MoveList moves = findMoves(isMaximizingPlayer);

    // Check for memory allocation failure
    if (moves.moves == NULL && moves.size > 0) {
        fprintf(stderr, "CRITICAL: findMoves() allocation failed in minimax\n");
        Move emergency = (Move){0};
        emergency.score = eval_position();
        return emergency;
    }

    // Terminal position: no legal moves
    if (moves.size == 0) {
        Move terminal = (Move){0};
        int inCheck = isCheck(isMaximizingPlayer);
        if (inCheck) {
            // Checkmate: prefer quicker mates by making score depth-dependent
            // If White is checkmated (isMaximizingPlayer=1), return large negative
            // If Black is checkmated (isMaximizingPlayer=0), return large positive
            int plyFromRoot = (rootSearchDepth - depth);
            if (plyFromRoot < 0) plyFromRoot = 0; // safety
            terminal.score = isMaximizingPlayer ? (NINF + plyFromRoot) : (INF - plyFromRoot);
        } else {
            // Stalemate
            terminal.score = 0;
        }
        if (moves.moves) free(moves.moves);
        return terminal;
    }

    // Enhanced Move Ordering: MVV ordering for captures, then non-captures
    BitBoard enemyPieces = isMaximizingPlayer ? Black : White;
    // Partition captures to the front
    int capCount = 0;
    for (int i = 0; i < moves.size; ++i) {
        if (moves.moves[i].Square & enemyPieces) {
            if (i != capCount) {
                Move tmp = moves.moves[capCount];
                moves.moves[capCount] = moves.moves[i];
                moves.moves[i] = tmp;
            }
            capCount++;
        }
    }
    // Sort captures by victim value descending
    for (int i = 0; i < capCount - 1; ++i) {
        for (int j = i + 1; j < capCount; ++j) {
            int victimI = piece_value_from_type(find_type(moves.moves[i].Square));
            int victimJ = piece_value_from_type(find_type(moves.moves[j].Square));
            if (victimJ > victimI) {
                Move tmp = moves.moves[i];
                moves.moves[i] = moves.moves[j];
                moves.moves[j] = tmp;
            }
        }
    }

    for (int i = 0; i < moves.size; i++) {
        Move move = moves.moves[i];
        
        movesEvaluated++;  // Increment counter for each move evaluated

        // Save board state before making the move
        BoardState savedState;
        savedState.White_pawns = White_pawns;
        savedState.White_knights = White_knights;
        savedState.White_rooks = White_rooks;
        savedState.White_bishops = White_bishops;
        savedState.White_queen = White_queen;
        savedState.White_king = White_king;
        savedState.Black_pawns = Black_pawns;
        savedState.Black_knights = Black_knights;
        savedState.Black_rooks = Black_rooks;
        savedState.Black_bishops = Black_bishops;
        savedState.Black_queen = Black_queen;
        savedState.Black_king = Black_king;
        
        // Snapshot castling rights
        int WcL = WcastleL, WcR = WcastleR, BcL = BcastleL, BcR = BcastleR;
        
        // Save move history count to restore after (since move_piece calls StoreMove)
        int savedCount = count;

        // Make move (skip if move is somehow invalid, though findMoves() should filter)
        int made = makeMove(move, isMaximizingPlayer);
        if (!made) {
            // Ensure full restore in case any side effects occurred
            White_pawns = savedState.White_pawns;
            White_knights = savedState.White_knights;
            White_rooks = savedState.White_rooks;
            White_bishops = savedState.White_bishops;
            White_queen = savedState.White_queen;
            White_king = savedState.White_king;
            Black_pawns = savedState.Black_pawns;
            Black_knights = savedState.Black_knights;
            Black_rooks = savedState.Black_rooks;
            Black_bishops = savedState.Black_bishops;
            Black_queen = savedState.Black_queen;
            Black_king = savedState.Black_king;
            updateAll();
            WcastleL = WcL; WcastleR = WcR; BcastleL = BcL; BcastleR = BcR;
            count = savedCount;
            continue;
        }

        Move eval = minimax(depth - 1, alpha, beta, !isMaximizingPlayer);

        // Hanging piece penalty: evaluate destination of our move after child search restore
        int penalty = hanging_penalty(move.Piece, move.Square, isMaximizingPlayer);
        if (penalty) {
            // Since eval.score is White-centric, adjust according to side
            if (isMaximizingPlayer) {
                eval.score -= penalty;
            } else {
                eval.score += penalty; // Black minimizing white's score; increase score hurts Black choice later
            }
        }
        // Restore board state
        White_pawns = savedState.White_pawns;
        White_knights = savedState.White_knights;
        White_rooks = savedState.White_rooks;
        White_bishops = savedState.White_bishops;
        White_queen = savedState.White_queen;
        White_king = savedState.White_king;
        Black_pawns = savedState.Black_pawns;
        Black_knights = savedState.Black_knights;
        Black_rooks = savedState.Black_rooks;
        Black_bishops = savedState.Black_bishops;
        Black_queen = savedState.Black_queen;
        Black_king = savedState.Black_king;
        updateAll();
        
        // Restore castling rights
        WcastleL = WcL; WcastleR = WcR; BcastleL = BcL; BcastleR = BcR;
        
        // Restore move history count (undo any StoreMove calls during search)
        count = savedCount;

        if (isMaximizingPlayer) {
            // White maximizes the score
            if (eval.score > bestScore) {
                bestScore = eval.score;
                bestMove = move;
                bestMove.score = bestScore;
                foundValidMove = 1;
                // Update alpha to the best score found so far
                if (bestScore > alpha) alpha = bestScore;
            }
        } else {
            // Black minimizes the score (since eval is from White's perspective)
            if (eval.score < bestScore) {
                bestScore = eval.score;
                bestMove = move;
                bestMove.score = bestScore;
                foundValidMove = 1;
                // Update beta to the best score found so far
                if (bestScore < beta) beta = bestScore;
            }
        }

        if (beta <= alpha) {
            break;  // Alpha-beta pruning
        }
    }

    // Safety check: if no move was better (shouldn't happen but prevents crashes)
    if (!foundValidMove && moves.size > 0) {
        bestMove = moves.moves[0];
        bestMove.score = bestScore;
    }

    if (moves.moves) free(moves.moves);
    return bestMove;
}

// Calculate adaptive search depth based on game phase
int getAdaptiveDepth() {
    // Count total pieces on the board (excluding kings)
    int whitePieces = countSetBits(White_pawns) + countSetBits(White_knights) + 
                      countSetBits(White_rooks) + countSetBits(White_bishops) + 
                      countSetBits(White_queen);
    int blackPieces = countSetBits(Black_pawns) + countSetBits(Black_knights) + 
                      countSetBits(Black_rooks) + countSetBits(Black_bishops) + 
                      countSetBits(Black_queen);
    int totalPieces = whitePieces + blackPieces;
    
    // Game phase determination (optimized for speed):
    // Opening/Early game: 26-32 pieces → depth 3 (many moves)
    // Midgame: 13-25 pieces → depth 4 (balanced)
    // Endgame: 1-12 pieces → depth 5-6 (fewer moves, can search deeper)
    
    if (totalPieces >= 26) {
        return BASE_DEPTH -1 ;  // Opening: many pieces, keep it fast
    } else if (totalPieces >= 21) {
        return BASE_DEPTH+ 1;  // Early midgame: still many moves
    } else if (totalPieces >= 13) {
        return BASE_DEPTH + 1;  // Midgame
    } else if (totalPieces >= 8) {
        return BASE_DEPTH + 1;  // Early endgame: depth 5
    } else if (totalPieces >= 4) {
        return BASE_DEPTH + 2;  // Late endgame: depth 6
    } else {
        return BASE_DEPTH + 3;  // King vs King + piece: depth 7
    }
}

Move bestMove(int isWhite) {

    int depth = getAdaptiveDepth();  // Use adaptive depth instead of fixed
    rootSearchDepth = depth;
    movesEvaluated = 0;  // Reset counter before search
    
    // Calculate total pieces for phase display
    int totalPieces = countSetBits(White_pawns | White_knights | White_rooks | 
                                   White_bishops | White_queen | Black_pawns | 
                                   Black_knights | Black_rooks | Black_bishops | Black_queen);
    const char* phase = (totalPieces >= 20) ? "Opening/Midgame" : 
                        (totalPieces >= 13) ? "Midgame" : 
                        (totalPieces >= 8) ? "Endgame" : "Late Endgame";
    
    printf("=== AI Search: %s (depth %d, %d pieces) ===\n", phase, depth, totalPieces);
    //printf("Position eval: %d\n", eval_position());
    
    Move result = minimax(depth,NINF, INF, isWhite);
    // Print stats
    printf("Search complete! Nodes: %d (%.1fM), Best score: %d\n", 
           movesEvaluated, movesEvaluated / 1000000.0, result.score);
    
    // For PV leaf nodes, replace score with immediate position eval after move
    if (result.Piece && result.Square) {
        BoardState savedState;
        savedState.White_pawns = White_pawns;
        savedState.White_knights = White_knights;
        savedState.White_rooks = White_rooks;
        savedState.White_bishops = White_bishops;
        savedState.White_queen = White_queen;
        savedState.White_king = White_king;
        savedState.Black_pawns = Black_pawns;
        savedState.Black_knights = Black_knights;
        savedState.Black_rooks = Black_rooks;
        savedState.Black_bishops = Black_bishops;
        savedState.Black_queen = Black_queen;
        savedState.Black_king = Black_king;
        int savedWcL = WcastleL, savedWcR = WcastleR, savedBcL = BcastleL, savedBcR = BcastleR;
        int savedCount = count;

        // Apply the move once to see the immediate position
        if (makeMove(result, isWhite)) {
            int immediateEval = eval_position();
            // Undo (manual restore rather than unmakeMove to avoid history side effects)
            White_pawns = savedState.White_pawns;
            White_knights = savedState.White_knights;
            White_rooks = savedState.White_rooks;
            White_bishops = savedState.White_bishops;
            White_queen = savedState.White_queen;
            White_king = savedState.White_king;
            Black_pawns = savedState.Black_pawns;
            Black_knights = savedState.Black_knights;
            Black_rooks = savedState.Black_rooks;
            Black_bishops = savedState.Black_bishops;
            Black_queen = savedState.Black_queen;
            Black_king = savedState.Black_king;
            WcastleL = savedWcL; WcastleR = savedWcR; BcastleL = savedBcL; BcastleR = savedBcR;
            count = savedCount;
            updateAll();
            // Replace score with immediate evaluation
            result.score = immediateEval;
            printf("Immediate position eval after move: %d (replacing PV leaf score)\n", immediateEval);
        } else {
            // If applying the move failed for some reason, restore state anyway
            White_pawns = savedState.White_pawns;
            White_knights = savedState.White_knights;
            White_rooks = savedState.White_rooks;
            White_bishops = savedState.White_bishops;
            White_queen = savedState.White_queen;
            White_king = savedState.White_king;
            Black_pawns = savedState.Black_pawns;
            Black_knights = savedState.Black_knights;
            Black_rooks = savedState.Black_rooks;
            Black_bishops = savedState.Black_bishops;
            Black_queen = savedState.Black_queen;
            Black_king = savedState.Black_king;
            WcastleL = savedWcL; WcastleR = savedWcR; BcastleL = savedBcL; BcastleR = savedBcR;
            count = savedCount;
            updateAll();
        }
    }

    return result;

}



#ifndef NO_MAIN
void compMove(int isWhite) {
    printf("\n%s (AI) is thinking...\n", isWhite ? "White" : "Black");
    Move move = bestMove(isWhite);
    
    if (move.Piece == 0 && move.Square == 0) {
        printf("AI has no legal moves!\n");
        return;
    }
    
    int result = makeMove(move,isWhite);
    if (result == 1) {
        printf("AI moved successfully.\n");
    } else {
        printf("ERROR: AI's makeMove failed! Result: %d\n", result);
    }

}


void userplay() {

    for(int i = 0; i < MAX_MOVES; i++ ) {

        printf("PLAYER 1 GO\n");
        userMove(1);
        printf("Player 2 Go\n");
        userMove(0);
    }


}
// Old main function - now renamed to avoid conflicts with frontend
int main() {
    int isWhite = 0;
    initPieceBitboards();

    printf("========== Chess Engine ==========\n");
    printf("Please Select White or Black (1 for White, 0 for Black): ");
    scanf("%d",&isWhite);

    printf("\nStarting position:\n");
    printBinary(Main);

    for(int i = 0; i < MAX_MOVES; i++) {
        if(isWhite) {
            userMove(1);
            printf("\nBoard after your move:\n");
            printBinary(Main);
            compMove(0);
            printf("\nBoard after AI move:\n");
            printBinary(Main);
        }else{
            compMove(1);
            printf("\nBoard after AI move:\n");
            printBinary(Main);
            userMove(0);
            printf("\nBoard after your move:\n");
            printBinary(Main);
        }
    }
    
    return 0;
}
#endif // NO_MAIN