// binary_printer.h
#ifndef BINARY_PRINTER_H
#define BINARY_PRINTER_H

#include <stdio.h>
#include <stdint.h>

void printBinary(uint64_t num) {
    int i;
    int size = sizeof(num) * 8; // Size of the number in bits

    for (i = 0; i < size; i++) {
        // Use bitwise shift and bitwise AND to print each bit
        printf("%ld", (num >> (size - 1 - i)) & 1);
        
        // Insert a space for better readability
        if ((i + 1) % 8 == 0)
            printf("\n");
    }

    printf("\n");
}

int countSetBits(uint64_t board) {
    int count = 0;
    while (board) {
        count += board & 1;
        board >>= 1;
    }
    return count;
}

uint64_t getLSB(uint64_t board) {
   return board & -board;

}

uint64_t removeLSB(uint64_t board) {
    return board & (board -1);
} 

uint64_t findFile(uint64_t piece) {

    if(piece & FileA) {
        return FileA;
    } 
    if (piece & FileB) {
        return FileB;
    }
    if (piece & FileC) {
        return FileC;
    }
    if(piece & FileD) {
        return FileD;
    }
    if(piece & FileE) {
        return FileE;
    }
    if(piece & FileF) {
        return FileF;
    }
    if(piece & FileG) {
        return FileG;
    }
    if(piece & FileH){
        return FileH;
    }
    return 0;

}

uint64_t findRank(uint64_t piece) {

    if ((piece & Rank1) != 0) {
        return Rank1;
    } 
    if ((piece & Rank2) != 0) {
        return Rank2;
    }
    if ((piece & Rank3) != 0) {
        return Rank3;
    }
    if ((piece & Rank4) != 0 ) {
        return Rank4;
    }
    if ((piece & Rank5) != 0) {
        return Rank5;
    }
    if ((piece & Rank6) != 0) {
        return Rank6;
    }
    if ((piece & Rank7) != 0) {
        return Rank7;
    }
    if ((piece & Rank8) != 0) {
        return Rank8;
    }
    return 0;
}

uint64_t findDia(uint64_t piece) {
    if ((piece & Dia1) != 0) {
        return Dia1;
    } 
    if ((piece & Dia2) != 0) {
        return Dia2;
    }
    if ((piece & Dia3) != 0) {
        return Dia3;
    }
    if ((piece & Dia4) != 0) {
        return Dia4;
    }
    if ((piece & Dia5) != 0) {
        return Dia5;
    }
    if ((piece & Dia6) != 0) {
        return Dia6;
    }
    if ((piece & Dia7) != 0) {
        return Dia7;
    }
    if ((piece & Dia8) != 0) {
        return Dia8;
    }
    if ((piece & Dia9) != 0) {
        return Dia9;
    }
    if ((piece & Dia10) != 0) {
        return Dia10;
    }
    if ((piece & Dia11) != 0) {
        return Dia11;
    }
    if ((piece & Dia12) != 0) {
        return Dia12;
    }
    if ((piece & Dia13) != 0) {
        return Dia13;
    }
    if ((piece & Dia14) != 0) {
        return Dia14;
    }
    if ((piece & Dia15) != 0) {
        return Dia15;
    }
    return 0;
}

uint64_t findADia(uint64_t piece) {
    if ((piece & ADia1) != 0) {
        return ADia1;
    } 
    if ((piece & ADia2) != 0) {
        return ADia2;
    }
    if ((piece & ADia3) != 0) {
        return ADia3;
    }
    if ((piece & ADia4) != 0) {
        return ADia4;
    }
    if ((piece & ADia5) != 0) {
        return ADia5;
    }
    if ((piece & ADia6) != 0) {
        return ADia6;
    }
    if ((piece & ADia7) != 0) {
        return ADia7;
    }
    if ((piece & ADia8) != 0) {
        return ADia8;
    }
    if ((piece & ADia9) != 0) {
        return ADia9;
    }
    if ((piece & ADia10) != 0) {
        return ADia10;
    }
    if ((piece & ADia11) != 0) {
        return ADia11;
    }
    if ((piece & ADia12) != 0) {
        return ADia12;
    }
    if ((piece & ADia13) != 0) {
        return ADia13;
    }
    if ((piece & ADia14) != 0) {
        return ADia14;
    }
    if ((piece & ADia15) != 0) {
        return ADia15;
    }
    return 0;
}
//Note this function was found on StackOver Flow (Dont understand it)
uint64_t reverse(uint64_t b) {
    b = (b & 0x5555555555555555) << 1 | ((b >> 1) & 0x5555555555555555);
    b = (b & 0x3333333333333333) << 2 | ((b >> 2) & 0x3333333333333333);
    b = (b & 0x0f0f0f0f0f0f0f0f) << 4 | ((b >> 4) & 0x0f0f0f0f0f0f0f0f);
    b = (b & 0x00ff00ff00ff00ff) << 8 | ((b >> 8) & 0x00ff00ff00ff00ff);

    return (b << 48) | ((b & 0xffff0000) << 16) | ((b >> 16) & 0xffff0000) | (b >> 48);
}

//Dont use this
uint64_t flipRank(uint64_t num) {
    uint64_t Board = 0;
    uint64_t Rank = findRank(num);
    if(num & FileA) {
        Board = Board | FileH;
    }
    if(num&FileB) {
        Board = Board | FileG;
    }
    if(num&FileC) {
        Board = Board | FileF;
    }
    if(num&FileD) {
        Board = Board | FileE;
    }
    if(num&FileE) {
        Board = Board | FileD;
    }
    if(num&FileF) {
        Board = Board | FileC;
    }
    if(num&FileG) {
        Board = Board | FileB;
    }
    if(num&FileH) {
        Board = Board | FileA;
    }
    return Board & Rank;
}




#endif /* BINARY_PRINTER_H */