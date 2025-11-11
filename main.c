#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "globals.h"
#include "moves.h"
#include "binary_printer.h"
#include "position_tables.h"
typedef uint64_t BitBoard;

#define MAX_MOVES 1000000
#define BASE_DEPTH 4  // Base depth for midgame, will be adjusted based on game phase

int WcastleL = 1;
int WcastleR = 1;
int BcastleL = 1;
int BcastleR = 1;


#define INF 2147483647
#define NINF -2147483647 

#define PondValue 100
#define RookValue 500
#define BishopValue 320
#define QueenValue 900
#define KnightValue 300
//Init Main Boards, all of the white pieces,black pieces, and all pieces
BitBoard Main;
BitBoard White;
BitBoard Black;

//Init White BitBoards
BitBoard White_ponds;
BitBoard White_knights;
BitBoard White_rooks; 
BitBoard White_bishops;
BitBoard White_queen;
BitBoard White_king;

//Init Black BitBoards
BitBoard Black_ponds;
BitBoard Black_knights;
BitBoard Black_rooks; 
BitBoard Black_bishops;
BitBoard Black_queen;
BitBoard Black_king;

int moveCount = 0;
int movesEvaluated = 0;  // Counter for moves evaluated during search

int find_type(BitBoard piece) {

    if((piece & Main) == 0) {
        //printf("find_type: piece 0x%llx not in Main 0x%llx\n", piece, Main);
        return 0;
    }
    // Check for pond
        if (piece & (White_ponds | Black_ponds)) {
        return 1; // Pond
    }
    
    // Check for knight
    if (piece & (White_knights | Black_knights)) {
        return 2; // Knight
    }

    // Check for bishop
    if (piece & (White_bishops | Black_bishops)) {
        return 3; // Bishop
    }

    // Check for rook
    if (piece & (White_rooks | Black_rooks)) {
        return 4; // Rook
    }

    // Check for queen
    if (piece & (White_queen | Black_queen)) {
        return 5; // Queen
    }

    // Check for king
    if (piece & (White_king | Black_king)) {
        return 6; // King
    }

    // Default case: fails
    return -1;
}



void updateAll();

// BoardState, Move, and MoveList are now defined in globals.h

Move MoveHistory[MAX_MOVES];
int count;

typedef struct {
    Move bestMove;
    int score;
} MoveScore;




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


void printFlags() {
     printf("WL:%d\nWR:%d\nBL:%d\nBR:%d\n",WcastleL,WcastleR,BcastleL,BcastleR);
}
    

void updateMain(){
    Main = White | Black;
}
void updateWhite() {
White = White_ponds | White_knights | White_rooks | White_bishops | White_king | White_queen;

}
void updateBlack(){
    Black = Black_ponds | Black_knights | Black_rooks | Black_bishops | Black_king | Black_queen;
}
void updateAll() {
    updateBlack();
    updateWhite();
    updateMain();
}

void reset() {
     White_ponds = A2 | B2 | C2 | D2 | E2 | F2 | G2 | H2;
    White_knights = B1 | G1;
    White_rooks = H1 | A1;
    White_bishops = C1 | F1;
    White_queen = D1;
    White_king = E1;


    Black_ponds = A7 | B7 | C7 | D7 | E7 | F7 | G7 | H7;
    Black_knights = B8 | G8;
    Black_rooks = H8 | A8;
    Black_bishops = C8 | F8;
    Black_queen = D8;
    Black_king = E8;
    WcastleL = 1;
    WcastleR = 1;
    BcastleL = 1;
    BcastleR = 1;  

    updateAll();
}

void testsetup2() {

    Black_king = E8;
    Black_rooks = H8 | A8;

    White_rooks = H1 | A1;
    White_king = E1;

    updateAll();
    //Store the first board as the first move
    Move temp;
    temp.Bblack = Black;
    temp.Bwhite = White;
    MoveHistory[0] = temp;

}

void reset1() {
    White_ponds = A2 | B2 | C2 | D2 | E2 | F2 | G2 | H2;
    White_knights = B1 | G1;
    White_rooks = H1 | A1;
    White_bishops = C1 | F1;
    White_queen = D1;
    White_king = E1;


    Black_ponds = A7 | B7 | C7 | D7 | E7 | F7 | G7 | H7;
    Black_knights = B8 | G8;
    Black_rooks = H8 | A8;
    Black_bishops = C8 | F8;
    Black_queen = D8;
    Black_king = E8;
    WcastleL = 1;
    WcastleR = 1;
    BcastleL = 1;
    BcastleR = 1;  

    updateAll();
}

void initPieceBitboards() {
    White_ponds = A2 | B2 | C2 | D2 | E2 | F2 | G2 | H2;
    White_knights = B1 | G1;
    White_rooks = H1 | A1;
    White_bishops = C1 | F1;
    White_queen = D1;
    White_king = E1;


    Black_ponds = A7 | B7 | C7 | D7 | E7 | F7 | G7 | H7;
    Black_knights = B8 | G8;
    Black_rooks = H8 | A8;
    Black_bishops = C8 | F8;
    Black_queen = D8;
    Black_king = E8;

    

    updateAll();
    Move temp;
    temp.Bblack = Black;
    temp.Bwhite = White;
    MoveHistory[0] = temp;
    
}

void castletest() {
    White_rooks = A1;
    White_king = E1;


    updateAll();
    Move temp;
    temp.Bblack = Black;
    temp.Bwhite = White;
    MoveHistory[0] = temp;
    count = 1;
}


void King_cap(int isWhite){
    printf("Game Over:");
    isWhite ? (printf("White Wins")) : (printf("Black Wins"));

}

BitBoard fileAttacks(BitBoard occ, BitBoard fileMaskEx, BitBoard bitMask) {
    BitBoard forward, reversed;
    forward = occ & fileMaskEx;
    reversed = reverse(forward); 
    
    forward -= bitMask;
    reversed -= reverse(bitMask); 
    forward ^= reverse(reversed); 
    
    forward &= fileMaskEx;
    
    return forward;
}






//Returns all vaild moves for a king (Not done, need to check if the king is in check or checkmated)
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

BitBoard find_pondmoves(BitBoard Piece, int isWhite) {

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

Move find_pondmoveup(BitBoard Piece, int isWhite) {
    Move move;
    BitBoard vaild_moves = ~(White|Black); 
    move.Square = (isWhite ? (Piece << 8) : (Piece >> 8)) & vaild_moves;
    move.Piece = Piece;
    return move;

}
Move find_pondmovedown(BitBoard Piece,int isWhite) {

    Move move;

    BitBoard vaild_moves = ~(White|Black);
    move.Square = (isWhite ? ((((Piece & Rank2) << 8 ) & (vaild_moves)) << 8) : ((((Piece & Rank7) >> 8 ) & (vaild_moves)) >> 8) )& vaild_moves;
    move.Piece = Piece;
    return move;
}
Move find_pondtakeright(BitBoard Piece, int isWhite) {

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
Move find_pondtakeleft(BitBoard Piece, int isWhite) {
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

Move find_tallLright(BitBoard Piece, int isWhite) {
    Move move;
    BitBoard vaild_moves = ~(White|Black); 
    move.Square = ((Piece & ClearFile_H) >> 17) & vaild_moves;
    move.Piece = Piece;
    return move;
    
}
Move find_tallLleft(BitBoard Piece, int isWhite) {
     Move move;
    BitBoard vaild_moves = ~(White|Black); 
    move.Square = ((Piece & ClearFile_A) >> 15) & vaild_moves;
    move.Piece = Piece;
    return move;
}
Move find_shortLright(BitBoard Piece,int isWhite) {
     Move move;
    BitBoard vaild_moves = ~(White|Black); 
    move.Square = ((Piece & ClearFile_G & ClearFile_H) >> 10) & vaild_moves;
    move.Piece = Piece;
    return move;
}
Move find_shortLleft(BitBoard Piece, int isWhite) {
    Move move;
    BitBoard vaild_moves = ~(White|Black); 
    move.Square = ((Piece & ClearFile_B & ClearFile_A) >> 6) & vaild_moves;
    move.Piece = Piece;
    return move;
}
Move find_downtallLright(BitBoard Piece, int isWhite) {
Move move;
    BitBoard vaild_moves = ~(White|Black); 
    move.Square = ((Piece & ClearFile_H) << 15) & vaild_moves;
    move.Piece = Piece;
    return move;
}
Move find_downtallLleft(BitBoard Piece, int isWhite) {
    Move move;
    BitBoard vaild_moves = ~(White|Black); 
    move.Square = ((Piece & ClearFile_A) << 17) & vaild_moves;
    move.Piece = Piece;
    return move;
}
Move find_downshortLright(BitBoard Piece, int isWhite){
    Move move;
    BitBoard vaild_moves = ~(White|Black); 
    move.Square = ((Piece & ClearFile_G & ClearFile_H) << 6) & vaild_moves;
    move.Piece = Piece;
    return move;
}
Move find_downshortLleft(BitBoard Piece, int isWhite) {
    Move move;
    BitBoard vaild_moves = ~(White|Black); 
    move.Square = ((Piece & ClearFile_B & ClearFile_A) << 10) & vaild_moves;
    move.Piece = Piece;
    return move;
}

BitBoard find_knightmoves(BitBoard Piece, int isWhite) {

    BitBoard tall_L_right = (Piece & ClearFile_H) >> 17;
    BitBoard tall_L_left = (Piece & ClearFile_A) >> 15;

    BitBoard short_L_right = (Piece & ClearFile_G & ClearFile_H) >> 10;
    BitBoard short_L_left = (Piece & ClearFile_B & ClearFile_A) >> 6;

    BitBoard downTL_right = (Piece & ClearFile_H) << 15;
    BitBoard downTL_left = (Piece & ClearFile_A) << 17;

    BitBoard downShort_L_right = (Piece & ClearFile_G & ClearFile_H) << 6;
    BitBoard downshort_L_left = (Piece & ClearFile_B & ClearFile_A) << 10;

    return (tall_L_left|tall_L_right|short_L_left|short_L_right|downTL_right|downTL_left|downshort_L_left|downShort_L_right) & (isWhite ? ~White:~Black); 

}


BitBoard help_Rook(BitBoard Piece, int isWhite) {

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
    
   
   
    
    return ((file_attc^right^left) & (isWhite ? (~White) : (~Black))); //works for no pieces in the way
    
}
BitBoard find_rookmoves(BitBoard Piece, int isWhite) {
    
    BitBoard tempmove = 0;
    BitBoard moves = 0;
        

    BitBoard Piece_LSB;
    BitBoard Temp = Piece;

        int size = countSetBits(Piece);
        
        if(size > 1) {
            for(int i = 0; i < size; i++) {
                //gets LSB to isolate the piece on the board
                Piece_LSB = getLSB(Temp);
                tempmove = help_Rook(Piece_LSB, isWhite);
                moves = tempmove | moves;
                //removes the LSB then get the next(gets moves for each rook on the board)
                Temp = removeLSB(Temp);

            }

        }else {
            
            
            BitBoard one = help_Rook(Piece,isWhite);
            
            return one;
        }


        return moves;


    }


BitBoard help_Bishop(BitBoard Piece, int isWhite) {

    BitBoard dia;
    BitBoard antiDia;

    dia = fileAttacks((Main& ~Piece),findDia(Piece),Piece);
    antiDia = fileAttacks((Main& ~Piece),findADia(Piece),Piece);
    return ((dia ^ antiDia) & (isWhite ? (~White): (~Black)));

}
BitBoard find_bishopmoves(BitBoard Piece, int isWhite) {

        BitBoard tempmove = 0;
        BitBoard moves = 0;
            

        BitBoard Piece_LSB;
        BitBoard Temp = Piece;

        int size = countSetBits(Piece);

        if(size > 1) {
            for(int i = 0; i < size; i++) {
                //gets LSB to isolate the piece on the board
                Piece_LSB = getLSB(Temp);
                tempmove = help_Bishop(Piece_LSB, isWhite);
                moves = tempmove | moves;
                //removes the LSB then get the next(gets moves for each rook on the board)
                Temp = removeLSB(Temp);

            }

        }else {
        
            BitBoard one = help_Bishop(Piece,isWhite);
            
            return one;
        }

        return moves;



    }

BitBoard find_queenmoves(BitBoard Piece, int isWhite) {

    return (find_bishopmoves(Piece, isWhite) ^ find_rookmoves(Piece,isWhite));
}

MoveList find_allponds(int isWhite) {

    Move tempmove;
    Move tempmove2;
    Move tempmove3;
    Move tempmove4;

    int size = countSetBits(isWhite ? White_ponds : Black_ponds);
    
    //Move moves[size*4];
    Move * moves = (Move*)malloc(sizeof(Move) * (size*4));
    
    if (moves == NULL) {
        fprintf(stderr, "ERROR: malloc failed in find_allponds\n");
        return (MoveList){.moves = NULL, .size = 0};
    }

    BitBoard Piece_LSB;
    BitBoard Temp = isWhite ? White_ponds : Black_ponds;

    int index = 0;  // Use a separate index variable

    for (int i = 0; i < size; i++) {
        //gets LSB to isolate the piece on the board
        Piece_LSB = getLSB(Temp);
        tempmove = find_pondmoveup(Piece_LSB, isWhite);
        tempmove2 = find_pondmovedown(Piece_LSB, isWhite);
        tempmove3 = find_pondtakeright(Piece_LSB, isWhite);
        tempmove4 = find_pondtakeleft(Piece_LSB, isWhite);
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

BitBoard vaildMovesWhite() {
    
    BitBoard ponds = find_pondmoves(White_ponds,1);
    
    BitBoard rooks = find_rookmoves(White_rooks,1);
    
    BitBoard knights = find_knightmoves(White_knights,1);
    
    BitBoard king = find_kingmoves(White_king,1);
    
    BitBoard queens = find_queenmoves(White_queen,1);
    BitBoard bishops = find_bishopmoves(White_bishops,1);
 

    return (ponds|rooks|knights|king|queens|bishops);
    
}
BitBoard vaildMovesBlack() {

    BitBoard ponds = find_pondmoves(Black_ponds,0);
    BitBoard rooks = find_rookmoves(Black_rooks,0);
    BitBoard knights = find_knightmoves(Black_knights,0);
    BitBoard king = find_kingmoves(Black_king,0);
    BitBoard queens = find_queenmoves(Black_queen,0);
    BitBoard bishops = find_bishopmoves(Black_bishops,0);

    return (ponds|rooks|knights|king|queens|bishops);
    
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
        BitBoard validMoves = help_Rook(rookSquare, isWhite);

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
        BitBoard validMoves = help_Bishop(bishopSquare, isWhite);

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
        BitBoard validMoves = help_Bishop(QueenSquare, isWhite) | help_Rook(QueenSquare,isWhite);

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
    Move tempmove;
    Move tempmove2;
    Move tempmove3;
    Move tempmove4;
    Move tempmove5;
    Move tempmove6;
    Move tempmove7;
    Move tempmove8;

    int size = countSetBits(isWhite ? White_knights : Black_knights);
    
    
    if(size == 0) {
        return (MoveList){0};

    }
    // Each knight can have up to 8 legal moves; allocate accordingly
    // Using size*8 prevents buffer overruns that could corrupt search state
    Move *moves = (Move *)malloc(sizeof(Move) * (size * 8));

    BitBoard Piece_LSB;
    BitBoard Temp = isWhite ? White_knights : Black_knights;

    int index = 0;  // Use a separate index variable

    for (int i = 0; i < size; i++) {
        //gets LSB to isolate the piece on the board
        Piece_LSB = getLSB(Temp);
        tempmove = find_tallLleft(Piece_LSB, isWhite);
        tempmove2 = find_tallLright(Piece_LSB, isWhite);
        tempmove3 = find_shortLleft(Piece_LSB, isWhite);
        tempmove4 = find_shortLright(Piece_LSB, isWhite);
        tempmove5 = find_downtallLleft(Piece_LSB, isWhite);
        tempmove6 = find_downtallLright(Piece_LSB, isWhite);
        tempmove7 = find_downshortLleft(Piece_LSB, isWhite);
        tempmove8 = find_downshortLright(Piece_LSB, isWhite);

        if (tempmove8.Square) {
            // Guard against any unforeseen overflow
            if (index >= size * 8) break;
            moves[index].Square = tempmove8.Square;
            moves[index].Piece = tempmove8.Piece;
            index++;
        }
        
        if (tempmove7.Square) {
            if (index >= size * 8) break;
            moves[index].Square = tempmove7.Square;
            moves[index].Piece = tempmove7.Piece;
            index++;
        }
        if (tempmove6.Square) {
            if (index >= size * 8) break;
            moves[index].Square = tempmove6.Square;
            moves[index].Piece = tempmove6.Piece;
            index++;
        }
        if (tempmove5.Square) {
            if (index >= size * 8) break;
            moves[index].Square = tempmove5.Square;
            moves[index].Piece = tempmove5.Piece;
            index++;
        }
        if (tempmove4.Square) {
            if (index >= size * 8) break;
            moves[index].Square = tempmove4.Square;
            moves[index].Piece = tempmove4.Piece;
            index++;
        }
        if (tempmove3.Square) {
            if (index >= size * 8) break;
            moves[index].Square = tempmove3.Square;
            moves[index].Piece = tempmove3.Piece;
            index++;
        }
        if (tempmove2.Square) {
            if (index >= size * 8) break;
            moves[index].Square = tempmove2.Square;
            moves[index].Piece = tempmove2.Piece;
            index++;
        }
        if (tempmove.Square) {
            if (index >= size * 8) break;
            moves[index].Square = tempmove.Square;
            moves[index].Piece = tempmove.Piece;
            index++;
        }
        //removes the LSB then get the next knight
        Temp = removeLSB(Temp);
    }

    MoveList movelist;
    movelist.moves = moves;
    movelist.size = index;

    return movelist;
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


Move castlemove(BitBoard Piece,int isWhite) {
    Move move;
    
    // CRITICAL FIX: Only allow castling if the rook is on its starting square
    // AND the corresponding castling right is still available
    if(isWhite) {
        // White queenside castle: rook must be on A1 and WcastleL must be true
        if((Piece == A1) && (WcastleL) && ((Main & castleWL) == 0)) {
            move.Piece = E1;
            move.Square = C1;
            move.castle = 1;
            return move;
        }
        // White kingside castle: rook must be on H1 and WcastleR must be true
        else if((Piece == H1) && (WcastleR) && ((Main & castleWR) == 0)) {
            move.Piece = E1;
            move.Square = G1;
            move.castle = 2;
            return move;
        }
    } else {
        // Black queenside castle: rook must be on A8 and BcastleL must be true
        if((Piece == A8) && (BcastleL) && ((Main & castleBL) == 0)) {
            move.Piece = E8;
            move.Square = C8;
            move.castle = 3;
            return move;
        }
        // Black kingside castle: rook must be on H8 and BcastleR must be true
        else if((Piece == H8) && BcastleR && ((Main & castleBR) == 0)) {
            move.Piece = E8;
            move.Square = G8;
            move.castle = 4;
            return move;
        }
    }
    
    // No valid castle move
    return (Move){0};
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

   
    if(isWhite ? (White_king & vaildMovesBlack()) : (Black_king & vaildMovesWhite())) {
        return 1;
    }else {
        return 0;
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
        moves.moves[i].prevState.White_ponds = White_ponds;
        moves.moves[i].prevState.White_knights = White_knights;
        moves.moves[i].prevState.White_rooks = White_rooks;
        moves.moves[i].prevState.White_bishops = White_bishops;
        moves.moves[i].prevState.White_queen = White_queen;
        moves.moves[i].prevState.White_king = White_king;
        moves.moves[i].prevState.Black_ponds = Black_ponds;
        moves.moves[i].prevState.Black_knights = Black_knights;
        moves.moves[i].prevState.Black_rooks = Black_rooks;
        moves.moves[i].prevState.Black_bishops = Black_bishops;
        moves.moves[i].prevState.Black_queen = Black_queen;
        moves.moves[i].prevState.Black_king = Black_king;
        
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
            White_ponds = moves.moves[i].prevState.White_ponds;
            White_knights = moves.moves[i].prevState.White_knights;
            White_rooks = moves.moves[i].prevState.White_rooks;
            White_bishops = moves.moves[i].prevState.White_bishops;
            White_queen = moves.moves[i].prevState.White_queen;
            White_king = moves.moves[i].prevState.White_king;
            Black_ponds = moves.moves[i].prevState.Black_ponds;
            Black_knights = moves.moves[i].prevState.Black_knights;
            Black_rooks = moves.moves[i].prevState.Black_rooks;
            Black_bishops = moves.moves[i].prevState.Black_bishops;
            Black_queen = moves.moves[i].prevState.Black_queen;
            Black_king = moves.moves[i].prevState.Black_king;
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

MoveList findMoves(int isWhite) {

    MoveList moves;
    MoveList pondmoves = find_allponds(isWhite);
    
    
    MoveList rookmoves = find_allrooks(isWhite);
  
    MoveList queenmoves = find_allqueens(isWhite);

    MoveList bishopmoves = find_allbishops(isWhite);
   
    MoveList kingmoves = find_allking(isWhite);
 

    MoveList knightmoves = find_allknights(isWhite);
    

    //rooks,queens,ponds
    MoveList RookPondQueen = combineMoveLists(combineMoveLists(pondmoves,rookmoves),queenmoves);
    if (rookmoves.moves) free(rookmoves.moves);
    if (pondmoves.moves) free(pondmoves.moves);
    if (queenmoves.moves) free(queenmoves.moves);

    MoveList KingKnightBishop = combineMoveLists(combineMoveLists(kingmoves,knightmoves),bishopmoves);
    if (kingmoves.moves) free(kingmoves.moves);
    if (knightmoves.moves) free(knightmoves.moves);
    if (bishopmoves.moves) free(bishopmoves.moves);
   
    moves = combineMoveLists(RookPondQueen,KingKnightBishop);
    if (RookPondQueen.moves) free(RookPondQueen.moves);
    if (KingKnightBishop.moves) free(KingKnightBishop.moves);
    
    

    MoveList validmoves = filterChecks(moves,isWhite); 
    free(moves.moves);
    
    return validmoves;
}




int take(BitBoard sqaure, int isWhite) {
    int type = find_type(sqaure);
   
    if(isWhite) {
        //for White Piece being taken
        switch (type)
        {
            case 1:
                White_ponds = (White_ponds & ~sqaure);
                break;
            case 2:
                White_knights = (White_knights & ~sqaure);
                break;
            case 3:
                White_bishops = (White_bishops & ~sqaure);
                break;
            case 4:
                White_rooks = (White_rooks & ~sqaure);
                // If a rook is captured on its starting square, disable that side's castling
                if(sqaure == H1) WcastleR = 0;
                if(sqaure == A1) WcastleL = 0;
                break;
            case 5:
                White_queen = (White_queen & ~sqaure);
                break;
            case 6: 
               // King_cap();
                break;
            default:
                break;
        }
    }else {
        //for Black Piece being taken
        switch (type)
        {
            case 1:
                Black_ponds = (Black_ponds & ~sqaure);
                break;
            case 2:
                Black_knights = (Black_knights & ~sqaure);
                break;
            case 3:
                Black_bishops = (Black_bishops & ~sqaure);
                break;
            case 4:
                Black_rooks = (Black_rooks & ~sqaure);
                // If a rook is captured on its starting square, disable that side's castling
                if(sqaure == H8) BcastleR = 0;
                if(sqaure == A8) BcastleL = 0;
                break;
            case 5:
                Black_queen = (Black_queen & ~sqaure);
                break;
            case 6: 
               // King_cap();
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
        move.Bblack = Black;
        move.Bwhite = White;

        //increases move count
        count++;
        //stores move
        MoveHistory[count] = move;
        
        

}

int move_piece(Move move, int isWhite) {
    BitBoard Piece = move.Piece;
    BitBoard sqaure = move.Square;
    //printf("Moving piece from %llx to %llx\n", Piece, sqaure);
    int type = find_type(Piece);
    
    if(type == 0) {
        // Piece not on board - move is invalid
        return -1;
    }
 
 

    int c = 0;
    // Castling detection: king moved two squares horizontally (avoid global all_VaildMoves())
    if(type == 6) {
        if((Piece >> 2) == sqaure) {
            c = isWhite ? 2 : 4; // king side
        } else if((Piece << 2) == sqaure) {
            c = isWhite ? 1 : 3; // queen side
        }
    }



////For Promotion of a pond, as of right now only can be a queen
    if(type == 1) {
        if((sqaure & Rank1) || (sqaure & Rank8)) {
            if(isWhite) {
                White_ponds = White_ponds & ~Piece;
                White_queen = White_queen | sqaure;
                StoreMove(move);
                return 1;
            }else {
                Black_ponds = Black_ponds & ~Piece;
                Black_queen = Black_queen | sqaure;
                StoreMove(move);
                return 1;
            }
        }
    }
//////
 
    // Removed expensive global all_VaildMoves() presence test; each piece case validates
    // the move with its own generator. Keeping the global union was a major bottleneck.

    if(type == 0) {
        //printf("Piece not found");
        return -1;
    }
   
    move.capturetype = 0;
    //Castling
    
    if(c == 1 || c == 2 || c ==3 || c ==4) {
        switch (c)
        {
        case 1:
            //A file White
            White_rooks = (White_rooks & ~A1) | D1;
            White_king = C1;
            //updateAll();
            StoreMove(move);
            WcastleR = 0;
            return 1;
            break;
        case 2:
            //H file White
            White_rooks = (White_rooks & ~H1) | F1;
            White_king = G1;
            //updateAll();
            StoreMove(move);
            WcastleL = 0;
            return 1;
            break;
        case 3: 
            //A file Black
            Black_rooks = (Black_rooks & ~A8) | D8;
            Black_king = C8;
            //updateAll();
            StoreMove(move);
            BcastleR = 0;
            return 1;
            break;
        case 4:
            //H file Black
            Black_rooks = (Black_rooks & ~H8) | F8;
            Black_king = G8;
            //updateAll();
            StoreMove(move);
            BcastleL = 0;
            return 1;
            break;
        default:
            break;
        }
    }


    

    if(isWhite) {
        switch (type)
        {

        case 1:
            if(sqaure & find_pondmoves(Piece, isWhite)) {
                if(sqaure &(isWhite ? Black:White)) {
                    //this means it capturing a piece and must update both boards
                    move.capturetype = take(sqaure, 0); //function to get rid of peice on that bitboard, and update the score
                    White_ponds = (White_ponds & ~Piece) | sqaure;
                    //updateAll();
                    StoreMove(move);
                    return 1;
                }else {
                    //not capturing
                    White_ponds = (White_ponds & ~Piece) | sqaure;
                    StoreMove(move);
                    //updateAll();
                    
                    return 1;
                    
                }
            } 
            break;
        case 2: 
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
        case 3:
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
        case 4:
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
        case 5:
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
        case 6:
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
        case 1:
            if(sqaure & find_pondmoves(Piece, isWhite)) {
                if(sqaure &(isWhite ? Black:White)) {
                    //this means it capturing a piece and must update both boards
                    move.capturetype = take(sqaure, 1); //function to get rid of peice on that bitboard, and update the score
                    Black_ponds = (Black_ponds & ~Piece) | sqaure;
                   // updateAll();
                    StoreMove(move);
                    return 1;
                }else {
                    //not capturing
                    Black_ponds = (Black_ponds & ~Piece) | sqaure;
                    //updateAll();
                    StoreMove(move);
                    return 1;
                }
            } 
            break;
        case 2: 
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
        case 3:
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
        case 4:
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
        case 5:
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
        case 6:
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

int makeMove(Move move, int isWhite) {
    // Capture board state BEFORE making the move
    move.prevState.White_ponds = White_ponds;
    move.prevState.White_knights = White_knights;
    move.prevState.White_rooks = White_rooks;
    move.prevState.White_bishops = White_bishops;
    move.prevState.White_queen = White_queen;
    move.prevState.White_king = White_king;
    move.prevState.Black_ponds = Black_ponds;
    move.prevState.Black_knights = Black_knights;
    move.prevState.Black_rooks = Black_rooks;
    move.prevState.Black_bishops = Black_bishops;
    move.prevState.Black_queen = Black_queen;
    move.prevState.Black_king = Black_king;

    // Also snapshot castling rights and history count to fully restore on invalid moves
    int savedWcL = WcastleL, savedWcR = WcastleR, savedBcL = BcastleL, savedBcR = BcastleR;
    int savedCount = count;

    if(move_piece(move,isWhite) == -1) {
        printf("Failed to move piece\n");
        return 0;
    }

    // Check if the move leaves the player in check
    if(isCheck(isWhite)) {
        //printf("Invalid move: %s would be in check!\n", isWhite ? "White" : "Black");
        // Undo the move
        White_ponds = move.prevState.White_ponds;
        White_knights = move.prevState.White_knights;
        White_rooks = move.prevState.White_rooks;
        White_bishops = move.prevState.White_bishops;
        White_queen = move.prevState.White_queen;
        White_king = move.prevState.White_king;
        Black_ponds = move.prevState.Black_ponds;
        Black_knights = move.prevState.Black_knights;
        Black_rooks = move.prevState.Black_rooks;
        Black_bishops = move.prevState.Black_bishops;
        Black_queen = move.prevState.Black_queen;
        Black_king = move.prevState.Black_king;
        // Restore castling rights and move history count that may have changed
        WcastleL = savedWcL; WcastleR = savedWcR; BcastleL = savedBcL; BcastleR = savedBcR;
        count = savedCount;
        updateAll();
        return 0;
    }

   //int test = checkmate(!isWhite);

    return 1;

    
}

void undocapture(Move made, int captured) {
//if captured currecly unmake black pieces so that means black moved last and took a white piece
    if(captured) {
        //means white was captured 
        switch (made.capturetype)
        {
            case 1:

                White_ponds = White_ponds | made.Square;
                break;
            case 2:
                White_knights = White_knights | made.Square;
                break;
            case 3:
                White_bishops = White_bishops | made.Square;
                break;
            case 4:
                White_rooks = White_rooks | made.Square;
                break;
            case 5: 
                White_queen = White_queen | made.Square;
                break;
            case 6:
                White_king = White_king | made.Square;
                break;
            default:
                break;

        }
    }else {
        switch (made.capturetype)
        {
            case 1:
                Black_ponds = Black_ponds | made.Square;
                break;
            case 2:
                Black_knights = Black_knights | made.Square;
                break;
            case 3:
                Black_bishops = Black_bishops | made.Square;
                break;
            case 4:
                Black_rooks = Black_rooks | made.Square;
                break;
            case 5: 
                Black_queen = Black_queen | made.Square;
                break;
            case 6:
                Black_king = Black_king | made.Square;
                break;
            default:
                break;

        }
        

    }

}

// Helper function to add positional bonuses for a bitboard
// Inline to reduce call overhead during evaluation
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

// Material + positional evaluation from White's perspective (positive favors White)
int eval_position() {
    int WhiteScore = 0;
    int BlackScore = 0;
    
    // Material evaluation
    int numberOfPondsWhite = countSetBits(White_ponds);
    int numberOfPondsBlack = countSetBits(Black_ponds);
    

    int numberOfKnightsWhite = countSetBits(White_knights);
    int numberOfKnightsBlack = countSetBits(Black_knights);
    int numberOfRooksWhite = countSetBits(White_rooks);
    int numberOfRooksBlack = countSetBits(Black_rooks);
    int numberOfQueensWhite = countSetBits(White_queen);
    int numberOfQueensBlack = countSetBits(Black_queen);
    int numberOfBishopsWhite = countSetBits(White_bishops);
    int numberOfBishopsBlack = countSetBits(Black_bishops);

    WhiteScore += (numberOfPondsWhite * PondValue);    
    WhiteScore += (numberOfKnightsWhite * KnightValue);    
    WhiteScore += (numberOfBishopsWhite * BishopValue);   
    WhiteScore += (numberOfRooksWhite * RookValue);      
    WhiteScore += (numberOfQueensWhite * QueenValue);    
    
    BlackScore += (numberOfPondsBlack * PondValue);
    BlackScore += (numberOfKnightsBlack * KnightValue);
    BlackScore += (numberOfBishopsBlack * BishopValue);
    BlackScore += (numberOfRooksBlack * RookValue);
    BlackScore += (numberOfQueensBlack * QueenValue);

    // Positional evaluation
    if (White_ponds) WhiteScore += eval_piece_positions(White_ponds, WHITE_PAWN_POSITION_TABLE);
    if (Black_ponds) BlackScore += eval_piece_positions(Black_ponds, BLACK_PAWN_POSITION_TABLE);
    
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

    return (WhiteScore - BlackScore);
}
    
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




Move minimax(int depth, int alpha, int beta, int isMaximizingPlayer) {
    Move bestMove = (Move){0};
    int bestScore = (isMaximizingPlayer) ? NINF : INF;
    int foundValidMove = 0;  

    // Safety check: at depth 5, we expect ~1-5M evaluations in midgame
    // 50M is a reasonable upper bound to prevent infinite loops from bugs
    if (movesEvaluated > 50000000) {
        fprintf(stderr, "WARNING: Search exceeded 50M nodes, aborting to prevent crash\n");
        Move emergency = (Move){0};
        emergency.score = eval_position();
        return emergency;
    }

    if (depth == 0) {
        Move dummyMove = (Move){0};
        int eval = eval_position();
        // eval_position returns a White-centric score (positive favors White).
        // We do NOT negate at the leaf: White maximizes this score and Black minimizes it.
        // If you switch to a negamax formulation later, you'd negate when changing sides.
        dummyMove.score = eval;
        return dummyMove;
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
            terminal.score = isMaximizingPlayer ? (NINF + depth) : (INF - depth);
        } else {
            // Stalemate
            terminal.score = 0;
        }
        if (moves.moves) free(moves.moves);
        return terminal;
    }

    // Move ordering: prioritize captures for better alpha-beta pruning
    // This can reduce nodes searched by 10-100x at depth 5
    BitBoard enemyPieces = isMaximizingPlayer ? Black : White;
    for (int i = 0; i < moves.size - 1; i++) {
        for (int j = i + 1; j < moves.size; j++) {
            int iCaptures = (moves.moves[i].Square & enemyPieces) ? 1 : 0;
            int jCaptures = (moves.moves[j].Square & enemyPieces) ? 1 : 0;
            
            // Swap if j is a capture and i is not (captures first)
            if (jCaptures > iCaptures) {
                Move temp = moves.moves[i];
                moves.moves[i] = moves.moves[j];
                moves.moves[j] = temp;
            }
        }
    }

    for (int i = 0; i < moves.size; i++) {
        Move move = moves.moves[i];
        
        movesEvaluated++;  // Increment counter for each move evaluated

        // CRITICAL FIX: Save complete board state BEFORE makeMove
        // This prevents corruption from StoreMove() calls inside move_piece()
        BoardState savedState;
        savedState.White_ponds = White_ponds;
        savedState.White_knights = White_knights;
        savedState.White_rooks = White_rooks;
        savedState.White_bishops = White_bishops;
        savedState.White_queen = White_queen;
        savedState.White_king = White_king;
        savedState.Black_ponds = Black_ponds;
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
            White_ponds = savedState.White_ponds;
            White_knights = savedState.White_knights;
            White_rooks = savedState.White_rooks;
            White_bishops = savedState.White_bishops;
            White_queen = savedState.White_queen;
            White_king = savedState.White_king;
            Black_ponds = savedState.Black_ponds;
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

        // CRITICAL FIX: Restore board state manually (don't use unmakeMove which relies on corrupted MoveHistory)
        White_ponds = savedState.White_ponds;
        White_knights = savedState.White_knights;
        White_rooks = savedState.White_rooks;
        White_bishops = savedState.White_bishops;
        White_queen = savedState.White_queen;
        White_king = savedState.White_king;
        Black_ponds = savedState.Black_ponds;
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
    int whitePieces = countSetBits(White_ponds) + countSetBits(White_knights) + 
                      countSetBits(White_rooks) + countSetBits(White_bishops) + 
                      countSetBits(White_queen);
    int blackPieces = countSetBits(Black_ponds) + countSetBits(Black_knights) + 
                      countSetBits(Black_rooks) + countSetBits(Black_bishops) + 
                      countSetBits(Black_queen);
    int totalPieces = whitePieces + blackPieces;
    
    // Game phase determination:
    // Opening/Early game: 26-32 pieces → depth 3-4 (too many moves, slower)
    // Midgame: 13-25 pieces → depth 5 (balanced)
    // Endgame: 1-12 pieces → depth 6-8 (fewer moves, can search deeper)
    
    if (totalPieces >= 26) {
        return BASE_DEPTH - 2;  // Opening: many pieces, many moves
    } else if (totalPieces >= 20) {
        return BASE_DEPTH - 1;  // Early midgame
    } else if (totalPieces >= 13) {
        return BASE_DEPTH;  // Midgame: use base depth (5)
    } else if (totalPieces >= 8) {
        return BASE_DEPTH + 1;  // Early endgame: fewer pieces, can go deeper
    } else if (totalPieces >= 4) {
        return BASE_DEPTH + 2;  // Late endgame: very few pieces
    } else {
        return BASE_DEPTH + 3;  // King vs King + piece: search very deep
    }
}

Move bestMove(int isWhite) {

    int depth = getAdaptiveDepth();  // Use adaptive depth instead of fixed
    movesEvaluated = 0;  // Reset counter before search
    
    // Calculate total pieces for phase display
    int totalPieces = countSetBits(White_ponds | White_knights | White_rooks | 
                                   White_bishops | White_queen | Black_ponds | 
                                   Black_knights | Black_rooks | Black_bishops | Black_queen);
    const char* phase = (totalPieces >= 20) ? "Opening/Midgame" : 
                        (totalPieces >= 13) ? "Midgame" : 
                        (totalPieces >= 8) ? "Endgame" : "Late Endgame";
    
    printf("=== AI Search: %s (depth %d, %d pieces) ===\n", phase, depth, totalPieces);
    printf("Position eval: %d\n", eval_position());
    
    Move result = minimax(depth,NINF, INF, isWhite);
    // Print stats
    printf("Search complete! Nodes: %d (%.1fM), Best score: %d\n", 
           movesEvaluated, movesEvaluated / 1000000.0, result.score);
    printf("Best move: from=0x%llx to=0x%llx\n", result.Piece, result.Square);
    // The score stored in result is the leaf (PV) evaluation after full depth search.
    // For GUI display we usually want the evaluation of the POSITION AFTER this move
    // (the root child position), not the distant leaf. We temporarily make the move,
    // evaluate, then undo it and overwrite result.score with the immediate evaluation.
    if (result.Piece && result.Square) {
        BoardState savedState;
        savedState.White_ponds = White_ponds;
        savedState.White_knights = White_knights;
        savedState.White_rooks = White_rooks;
        savedState.White_bishops = White_bishops;
        savedState.White_queen = White_queen;
        savedState.White_king = White_king;
        savedState.Black_ponds = Black_ponds;
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
            White_ponds = savedState.White_ponds;
            White_knights = savedState.White_knights;
            White_rooks = savedState.White_rooks;
            White_bishops = savedState.White_bishops;
            White_queen = savedState.White_queen;
            White_king = savedState.White_king;
            Black_ponds = savedState.Black_ponds;
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
            White_ponds = savedState.White_ponds;
            White_knights = savedState.White_knights;
            White_rooks = savedState.White_rooks;
            White_bishops = savedState.White_bishops;
            White_queen = savedState.White_queen;
            White_king = savedState.White_king;
            Black_ponds = savedState.Black_ponds;
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




int unmakeMove() {
    if(count == 0) {
        return -1;
    }
    
    // Restore all piece bitboards from the state saved BEFORE this move
    BoardState prev = MoveHistory[count].prevState;
    White_ponds = prev.White_ponds;
    White_knights = prev.White_knights;
    White_rooks = prev.White_rooks;
    White_bishops = prev.White_bishops;
    White_queen = prev.White_queen;
    White_king = prev.White_king;
    Black_ponds = prev.Black_ponds;
    Black_knights = prev.Black_knights;
    Black_rooks = prev.Black_rooks;
    Black_bishops = prev.Black_bishops;
    Black_queen = prev.Black_queen;
    Black_king = prev.Black_king;
    
    updateAll();
    
    // Clear the current move from history
    MoveHistory[count] = (Move){0};
    count--;
    
    return 1;
}

void tests1() {

    //Test 1: Move one piece from start update board
    initPieceBitboards(); 
    
    BitBoard start = Main;
    Move move1;
    move1.Piece = E2;
    move1.Square = E3;

    makeMove(move1,1);
    if(Main & E3) {
        printf("Test 1 Passed\n");
    }else {
        printf("Test 1 Failed\n");
    }
    //Test 2: Move one piece from start undo

    unmakeMove();


    if(Main == start) {
        printf("Test 2 Passed\n");
    }else {
        printf("Test 2 Failed\n");
    }

    //Test 3: capture a piece update board
        Move move;
        move.Piece = E2;
        move.Square = E4;
        makeMove(move,1);

        Move move2;
        move2.Piece = D7;
        move2.Square = D5;
        makeMove(move2,0);

        move.Piece = E4;
        move.Square = D5;
        makeMove(move,1);

        if(White_ponds & D5 && (Black_ponds & ~D5)) {
            printf("TEST 3 Passed\n");
        }else {
            printf("Test 3 failed\n");
        }


    //TEST 4:cature a piece then undo move 
    unmakeMove();
    //WHEN CAPTURED THE PIECES ARE NOT PUT BACK RIGHT BUT MAIN IS RIGHT
    if((Black_ponds & D5 ) && (White_ponds & E4) ){
        printf("TEST 4 Passed\n");
    }else {
        printf("Test 4 failed\n");
    }


}

void tests2() {
    //find all possible moves (this should be an int like there are 7 moves did it find all 7)
   
    testsetup2();
    MoveList blackmoves = findMoves(0);
    MoveList whitemoves = findMoves(1);
   
    
    
    int moves = whitemoves.size + blackmoves.size;
   

    if(moves == (26*2)) {
        printf("test 1 passed");

    }else {
        printf("Test 1 failed");
    }
    printf("\n////////////////////////////////////////////////////////////////////////////////////////////////////////\n");

    Move kingmove;
    kingmove.Piece = E1;
    kingmove.Square = E2;

    makeMove(kingmove,1);
    
    MoveList newwhitemoves = findMoves(1);

    MoveList newblackmoves = findMoves(0);
    
     moves = newwhitemoves.size + newblackmoves.size;



    if(moves == (26 + 34)) {
        printf("test 2 passed");

    }else {
        printf("Test 2 failed\n");
        printf("WHITE MOVES: %d\n",newwhitemoves.size);
        printf("Black MOVES: %d\n",newblackmoves.size);
        
    }


    printf("\n////////////////////////////////////////////////////////////////////////////////////////////////////\n");
    
    free(newblackmoves.moves);
    free(newwhitemoves.moves);
    free(whitemoves.moves);
    free(blackmoves.moves);


    //check if it thinks it can castle after it already moved it king 

    //check moves

    //checkmate
}

void tests3() {
    //THESE SHOULD TEST THE CHECK AND CHECKMATE OF THE GAME ASWELL AS THE UNDO WITH CHECKS (SCARED)
    
}
// Old main function - now renamed to avoid conflicts with frontend
int main_old() {
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