#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "moves.h"
#include "binary_printer.h"
typedef uint64_t BitBoard;

#define MAX_MOVES 1000000
#define DEPTH 3

int WcastleL = 1;
int WcastleR = 1;
int BcastleL = 1;
int BcastleR = 1;




#define INF 2147483647
#define NINF -2147483647 

#define PondValue 1
#define RookValue 5
#define BishopValue 3
#define QueenValue 9
#define KnightValue 3
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

int find_type(BitBoard piece) {

    if((piece & Main) == 0) {
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

typedef struct BoardState {
    BitBoard White_ponds;
    BitBoard White_knights;
    BitBoard White_rooks;
    BitBoard White_bishops;
    BitBoard White_queen;
    BitBoard White_king;
    BitBoard Black_ponds;
    BitBoard Black_knights;
    BitBoard Black_rooks;
    BitBoard Black_bishops;
    BitBoard Black_queen;
    BitBoard Black_king;
} BoardState;

typedef struct Move{

    
    BitBoard Bwhite;
    BitBoard Bblack;
    BitBoard Piece;
    BitBoard Square;
    int type;
    int capturetype;
    int score;
    int castle;
    BoardState prevState;  // Store complete board state before move

}Move;

typedef struct MoveList 
{
    Move* moves;
    int size;
}MoveList;


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
    BitBoard takeLeft = isWhite ? (((Piece & ClearFile_A) << 7) & enemyColour)
                                : (((Piece & ClearFile_H) >> 7) & enemyColour);
    BitBoard takeRight = isWhite ? (((Piece & ClearFile_H) << 9) & enemyColour)
                                 : (((Piece & ClearFile_A) >> 9) & enemyColour);
    
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

    // capture diagonally forward-right for the side to move
    move.Square = isWhite ? (((Piece & ClearFile_H) << 9) & enemyColour)
                          : (((Piece & ClearFile_A) >> 9) & enemyColour);
    move.Piece = Piece;

    return move;
}
Move find_pondtakeleft(BitBoard Piece, int isWhite) {
    Move move;
    BitBoard enemyColour = isWhite ? Black:White;
    // capture diagonally forward-left for the side to move
    move.Square = isWhite ? (((Piece & ClearFile_A) << 7) & enemyColour)
                          : (((Piece & ClearFile_H) >> 7) & enemyColour);
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

    BitBoard Piece_LSB;
    BitBoard Temp = isWhite ? White_ponds : Black_ponds;

    int index = 0;  // Use a separate index variable

    for (int i = 0; i < (size * 4); i++) {
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
        //removes the LSB then get the next
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
        
        if (numofmoves == 0) {
            // This rook has no moves; continue to next piece
        } else {
            for (int rank = 0; rank < 8; ++rank) {
                for (int file = 0; file < 8; ++file) {
                    BitBoard square = 1ULL << (rank * 8 + file);
                    if (square & validMoves) {
                        moves.moves[moves.size].Square = square;
                        moves.moves[moves.size].Piece = rookSquare;
                        moves.size++;
                        
                    }
                }
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
        
        if (numofmoves == 0) {
            // This bishop has no moves; continue
        } else {
            for (int rank = 0; rank < 8; ++rank) {
                for (int file = 0; file < 8; ++file) {
                    BitBoard square = 1ULL << (rank * 8 + file);
                    if (square & validMoves) {
                        moves.moves[moves.size].Square = square;
                        moves.moves[moves.size].Piece = bishopSquare;
                        moves.size++;
                        
                    }
                }
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
        
        if (numofmoves == 0) {
            // This queen has no moves; continue
        } else {
            for (int rank = 0; rank < 8; ++rank) {
                for (int file = 0; file < 8; ++file) {
                    BitBoard square = 1ULL << (rank * 8 + file);
                    if (square & validMoves) {
                        moves.moves[moves.size].Square = square;
                        moves.moves[moves.size].Piece = QueenSquare;
                        moves.size++;
                        
                    }
                }
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
    //Move moves[size*4];
    Move *moves = (Move *)malloc(sizeof(Move) * (size*4));

    BitBoard Piece_LSB;
    BitBoard Temp = isWhite ? White_knights : Black_knights;

    int index = 0;  // Use a separate index variable

    for (int i = 0; i < (size * 4); i++) {
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
            moves[index].Square = tempmove8.Square;
            moves[index].Piece = tempmove8.Piece;
            index++;
        }
        
        if (tempmove7.Square) {
            moves[index].Square = tempmove7.Square;
            moves[index].Piece = tempmove7.Piece;
            index++;
        }
        if (tempmove6.Square) {
            moves[index].Square = tempmove6.Square;
            moves[index].Piece = tempmove6.Piece;
            index++;
        }
        if (tempmove5.Square) {
            moves[index].Square = tempmove5.Square;
            moves[index].Piece = tempmove5.Piece;
            index++;
        }
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
        //removes the LSB then get the next
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
    BitBoard A = Piece & FileA;
    
    
    if(castle(isWhite)) {
        if(isWhite) {
            if(A) {
                move.Piece = E1;
                move.Square = C1;
                move.castle = 1;
                
            }else {
                //File H
                move.Piece = E1;
                move.Square = G1;
                move.castle = 2;
            }
        }else {
            if(A) {
                move.Piece = E8;
                move.Square = C8;
                move.castle = 3;
                
            }else {
                //File H
                move.Piece = E8;
                move.Square = G8;
                move.castle = 4;
            }

        }
    }else {
        return (Move){0};
    }
    return move;
}

MoveList combineMoveLists(MoveList list1, MoveList list2) {
    MoveList combinedList;

    int size = list1.size + list2.size;
    Move *moves = malloc(sizeof(Move) * (size));
    // Allocate memory for the combined moves
    combinedList.moves = moves;
    //(Move *)malloc(sizeof(Move) * (list1.size + list2.size));
    if (combinedList.moves == NULL) {
        // Handle allocation failure
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
    
    // if (move == NULL) {
    //     printf("ERROR: Memory allocation failed\n");
    //     return (MoveList){0};
    // }

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
        
        int ok = move_piece(moves.moves[i], isWhite);
        if (ok == 1) {
            // Check if the move doesn't result in a check
            if (!(isCheck(isWhite))) {
                move[index] = moves.moves[i];
                index++;
            }
            unmakeMove();
        }
        // restore castling rights regardless
        WcastleL = WcL; WcastleR = WcR; BcastleL = BcL; BcastleR = BcR;
    }

    validmoves.moves = move;
    validmoves.size = index;
   
    
    // Free memory if no valid moves found
    if (index == 0) {
        //free(validmoves.moves);
        return (MoveList){0};
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
    free(rookmoves.moves);
    free(pondmoves.moves);
    free(queenmoves.moves);

    MoveList KingKnightBishop = combineMoveLists(combineMoveLists(kingmoves,knightmoves),bishopmoves); 
    free(kingmoves.moves);
    free(knightmoves.moves);
    free(bishopmoves.moves);
   
    moves = combineMoveLists(RookPondQueen,KingKnightBishop); 
    free(RookPondQueen.moves);
    free(KingKnightBishop.moves);
    
    

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
    
    int type = find_type(Piece);
    
    if(type == 0) {
        // Piece not on board - move is invalid
        return -1;
    }
 
 

    int c;
        if((type == 6) && (sqaure & all_VaildMoves()) != 0) {
            if(((Piece >> 2) == sqaure))  {
                c = isWhite ? (2) : (4);
            }else if((Piece << 2) == sqaure) {
                c = isWhite ? (1) : (3);
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
 
    if((sqaure & all_VaildMoves()) == 0) {
        //printf("InVaild move, Sqaure trying to move to was not in ValidMoves bitboard\n");
        
        return -1;
    }

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
                    if(Piece & FileH) {
                        WcastleR = 0;
                       // BcastleR = 0;
                    }
                    if(Piece & FileA) {
                        WcastleL = 0;
                        //BcastleL = 0;
                    }
                    return 1;

                }else {
                    //not capturing
                    White_rooks = (White_rooks & ~Piece) | sqaure;
                    //updateAll();
                    StoreMove(move);
                    if(Piece & FileH) {
                        WcastleR = 0;
                    }
                    if(Piece & FileA) {
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
                    if(Piece & FileH) {
                        BcastleR = 0;
                    }
                    if(Piece & FileA) {
                        BcastleL = 0;
                    }
                    return 1;
                }else {
                    //not capturing
                    Black_rooks = (Black_rooks & ~Piece) | sqaure;
                   // updateAll();
                    StoreMove(move);
                    if(Piece & FileH) {
                        BcastleR = 0;
                    }
                    if(Piece & FileA) {
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

    if(move_piece(move,isWhite) == -1) {
        printf("Failed to move piece\n");
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



// Material-only evaluation from White's perspective (positive favors White)
int eval_position() {
    int WhiteScore = 0;
    int BlackScore = 0;
    //TODO:
    /*
        Add positional evaluation
            - Create piece-square tables for each piece type
            - Use these tables to add/subtract points based on piece positions
        Add points for checks and checkmates
            - Checkmate should add max points to the side delivering it
            - Being in check should subtract points from the side in check
    */


    WhiteScore += (countSetBits(White_ponds) * PondValue);
    WhiteScore += (countSetBits(White_knights) * KnightValue);
    WhiteScore += (countSetBits(White_rooks) * RookValue);
    WhiteScore += (countSetBits(White_queen) * QueenValue);
    WhiteScore += (countSetBits(White_bishops) * BishopValue);
    

    BlackScore += (countSetBits(Black_ponds) * PondValue);
    BlackScore += (countSetBits(Black_knights) * KnightValue);
    BlackScore += (countSetBits(Black_rooks) * RookValue);
    BlackScore += (countSetBits(Black_queen) * QueenValue);
    BlackScore += (countSetBits(Black_bishops) * BishopValue);

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
        if (valid == 1) {
            return 1;
        } else {
            printf("\n*** Invalid move! Try again. ***\n");
        }
    }

}




Move minimax(int depth, int alpha, int beta, int isMaximizingPlayer) {
    Move bestMove;
    int bestScore = (isMaximizingPlayer) ? NINF : INF;

    if (depth == 0) {
        Move dummyMove = (Move){0};
        int eval = eval_position();
        dummyMove.score = isMaximizingPlayer ? eval : -eval;
        return dummyMove;
    }

    MoveList moves = findMoves(isMaximizingPlayer);

    // Terminal position: no legal moves
    if (moves.size == 0) {
        Move terminal = (Move){0};
        int inCheck = isCheck(isMaximizingPlayer);
        terminal.score = inCheck ? (isMaximizingPlayer ? NINF + 1 : INF - 1) : 0;
        if (moves.moves) free(moves.moves);
        return terminal;
    }

    for (int i = 0; i < moves.size; i++) {
        Move move = moves.moves[i];

        // Snapshot castling rights
        int WcL = WcastleL, WcR = WcastleR, BcL = BcastleL, BcR = BcastleR;

        // Make move
        makeMove(move, isMaximizingPlayer);

        Move eval = minimax(depth - 1, alpha, beta, !isMaximizingPlayer);

        // Undo move
        unmakeMove();
        // Restore castling rights
        WcastleL = WcL; WcastleR = WcR; BcastleL = BcL; BcastleR = BcR;

        if (isMaximizingPlayer) {
            if (eval.score > bestScore) {
                bestScore = eval.score;
                bestMove = move;
                bestMove.score = bestScore;
            }
            if (eval.score > alpha) alpha = eval.score;
        } else {
            if (eval.score < bestScore) {
                bestScore = eval.score;
                bestMove = move;
                bestMove.score = bestScore;
            }
            if (eval.score < beta) beta = eval.score;
        }

        if (beta <= alpha) {
            break;
        }
    }

    if (moves.moves) free(moves.moves);
    return bestMove;
}


Move bestMove(int isWhite) {

    int depth = DEPTH;
    return minimax(depth,NINF, INF, isWhite);

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