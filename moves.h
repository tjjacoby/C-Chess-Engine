

    #define A1 0x0000000000000080ULL
    #define B1 0x0000000000000040ULL
    #define C1 0x0000000000000020ULL
    #define D1 0x0000000000000010ULL
    #define E1 0x0000000000000008ULL
    #define F1 0x0000000000000004ULL
    #define G1 0x0000000000000002ULL
    #define H1 0x0000000000000001ULL

    #define A2 0x0000000000008000ULL
    #define B2 0x0000000000004000ULL
    #define C2 0x0000000000002000ULL
    #define D2 0x0000000000001000ULL
    #define E2 0x0000000000000800ULL
    #define F2 0x0000000000000400ULL
    #define G2 0x0000000000000200ULL
    #define H2 0x0000000000000100ULL

    #define A3 0x0000000000800000ULL
    #define B3 0x0000000000400000ULL
    #define C3 0x0000000000200000ULL
    #define D3 0x0000000000100000ULL
    #define E3 0x0000000000080000ULL
    #define F3 0x0000000000040000ULL
    #define G3 0x0000000000020000ULL
    #define H3 0x0000000000010000ULL

    #define A4 0x0000000080000000ULL
    #define B4 0x0000000040000000ULL
    #define C4 0x0000000020000000ULL
    #define D4 0x0000000010000000ULL
    #define E4 0x0000000008000000ULL
    #define F4 0x0000000004000000ULL
    #define G4 0x0000000002000000ULL
    #define H4 0x0000000001000000ULL

    #define A5 0x0000008000000000ULL
    #define B5 0x0000004000000000ULL
    #define C5 0x0000002000000000ULL
    #define D5 0x0000001000000000ULL
    #define E5 0x0000000800000000ULL
    #define F5 0x0000000400000000ULL
    #define G5 0x0000000200000000ULL
    #define H5 0x0000000100000000ULL

    #define A6 0x0000800000000000ULL
    #define B6 0x0000400000000000ULL
    #define C6 0x0000200000000000ULL
    #define D6 0x0000100000000000ULL
    #define E6 0x0000080000000000ULL
    #define F6 0x0000040000000000ULL
    #define G6 0x0000020000000000ULL
    #define H6 0x0000010000000000ULL

    #define A7 0x0080000000000000ULL
    #define B7 0x0040000000000000ULL
    #define C7 0x0020000000000000ULL
    #define D7 0x0010000000000000ULL
    #define E7 0x0008000000000000ULL
    #define F7 0x0004000000000000ULL
    #define G7 0x0002000000000000ULL
    #define H7 0x0001000000000000ULL

    #define A8 0x8000000000000000ULL
    #define B8 0x4000000000000000ULL
    #define C8 0x2000000000000000ULL
    #define D8 0x1000000000000000ULL
    #define E8 0x0800000000000000ULL
    #define F8 0x0400000000000000ULL
    #define G8 0x0200000000000000ULL
    #define H8 0x0100000000000000ULL
    #define Full 18446744073709551615ULL

    #define Rank1 (A1|B1|C1|D1|E1|F1|G1|H1)
    #define Rank2 (A2|B2|C2|D2|E2|F2|G2|H2)
    #define Rank3 (A3|B3|C3|D3|E3|F3|G3|H3)
    #define Rank4 (A4|B4|C4|D4|E4|F4|G4|H4)
    #define Rank5 (A5|B5|C5|D5|E5|F5|G5|H5)
    #define Rank6 (A6|B6|C6|D6|E6|F6|G6|H6)
    #define Rank7 (A7|B7|C7|D7|E7|F7|G7|H7)
    #define Rank8 (A8|B8|C8|D8|E8|F8|G8|H8)


    #define FileA (A1 | A2 | A3 | A4 | A5 | A6 | A7 | A8)
    #define FileB (B1 | B2 | B3 | B4 | B5 | B6 | B7 | B8)
    #define FileC (C1 | C2 | C3 | C4 | C5 | C6 | C7 | C8)
    #define FileD (D1 | D2 | D3 | D4 | D5 | D6 | D7 | D8)
    #define FileE (E1 | E2 | E3 | E4 | E5 | E6 | E7 | E8)
    #define FileF (F1 | F2 | F3 | F4 | F5 | F6 | F7 | F8)
    #define FileG (G1 | G2 | G3 | G4 | G5 | G6 | G7 | G8)
    #define FileH (H1 | H2 | H3 | H4 | H5 | H6 | H7 | H8)


    ///////////////////////////////////////////////////////////////////////////////////////
    //Look-ups 


    #define ClearFile_A (Full & ~(A1 | A2 | A3 | A4 | A5 | A6 | A7 | A8))
    #define ClearFile_B (Full & ~(B1 | B2 | B3 | B4 | B5 | B6 | B7 | B8))
    #define ClearFile_C (Full & ~(C1 | C2 | C3 | C4 | C5 | C6 | C7 | C8))
    #define ClearFile_D (Full & ~(D1 | D2 | D3 | D4 | D5 | D6 | D7 | D8))
    #define ClearFile_E (Full & ~(E1 | E2 | E3 | E4 | E5 | E6 | E7 | E8))
    #define ClearFile_F (Full & ~(F1 | F2 | F3 | F4 | F5 | F6 | F7 | F8))
    #define ClearFile_G (Full & ~(G1 | G2 | G3 | G4 | G5 | G6 | G7 | G8))
    #define ClearFile_H (Full & ~(H1 | H2 | H3 | H4 | H5 | H6 | H7 | H8))

    /////////////////////////////////////////////////////////
    //Dianogal Masks

    #define Dia1  H1
    #define Dia2  (G1|H2)
    #define Dia3  (F1|G2|H3)
    #define Dia4  (E1|F2|G3|H4)
    #define Dia5  (D1|E2|F3|G4|H5)
    #define Dia6  (C1|D2|E3|F4|G5|H6)
    #define Dia7  (B1|C2|D3|E4|F5|G6|H7)
    #define Dia8  (A1|B2|C3|D4|E5|F6|G7|H8)
    #define Dia9  (A2|B3|C4|D5|E6|F7|G8)
    #define Dia10 (A3|B4|C5|D6|E7|F8)
    #define Dia11 (A4|B5|C6|D7|E8)
    #define Dia12 (A5|B6|C7|D8)
    #define Dia13 (A6|B7|C8)
    #define Dia14 (A7|B8)
    #define Dia15 A8

    //AntiDianogal Mask

    #define ADia1 A1
    #define ADia2 (B1|A2)
    #define ADia3 (C1|B2|A3)
    #define ADia4 (D1|C2|B3|A4)
    #define ADia5 (E1|D2|C3|B4|A5)
    #define ADia6 (F1|E2|D3|C4|B5|A6)
    #define ADia7 (G1|F2|E3|D4|C5|B6|A7)
    #define ADia8 (H1|G2|F3|E4|D5|C6|B7|A8)
    #define ADia9 (H2|G3|F4|E5|D6|C7|B8)
    #define ADia10 (H3|G4|F5|E6|D7|C8)
    #define ADia11 (H4|G5|F6|E7|D8)
    #define ADia12 (H5|G6|F7|E8)
    #define ADia13 (H6|G7|F8)
    #define ADia14 (H7|G8)
    #define ADia15 H8

    
    #define castleWR (F1|G1)
    #define castleWL (B1|C1|D1)
    #define castleBR (F8|G8)
    #define castleBL (B8|C8|D8)

    ////////////////////////////////////////////////////////////////////////////////////////

    #define Typepawn 1
    #define Typeknight 2
    #define Typebishop 3
    #define Typerook 4
    #define Typequeen 5
    #define Typeking 6


