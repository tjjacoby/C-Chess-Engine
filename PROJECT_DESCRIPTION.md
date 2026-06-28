# C Chess Engine: A Project in Bitboard-Based Search & Algorithmic Optimization

## Project Overview

I developed a full-featured chess engine from scratch in C, capable of playing competitive chess against human opponents. The engine implements a complete move generation system, legal move validation, and an AI opponent using the Minimax algorithm with alpha-beta pruning. Rather than using any external game engines or libraries, I built the entire system using only fundamental C concepts, bitwise operations, and algorithmic principles learned through Wikipedia and technical references.

The core innovation in this project is the use of **bitboards**—a technique where each piece type and board position is represented as a single 64-bit unsigned integer, mapping directly to the 64 squares of a chess board. This approach allows the engine to leverage fast bitwise operations (AND, OR, XOR, bit shifts) to generate moves and evaluate positions, making the engine significantly faster than naive implementations that use arrays or complex data structures.

## Technical Architecture & Approach

### Bitboards: A Compact Representation
A bitboard represents the chess board as a 64-bit integer where each bit corresponds to one square. For example, each piece type (pawns, knights, bishops, rooks, queens, kings) has its own bitboard:
- Bit set to 1 means a piece occupies that square
- Bit set to 0 means the square is empty

This representation enables lightning-fast board queries and move generation. Instead of iterating through a 2D array, I can determine which pieces are present, which squares are attacked, and what moves are legal using only bitwise operations—a technique I chose specifically because I had recently learned about bitwise operations and wanted to see them applied in a real software project.

### Move Generation & Legal Validation
The engine generates all pseudo-legal moves for each piece type:
- **Pawn moves**: Forward advances and diagonal captures (with promotion logic)
- **Piece moves**: Knights (8-move jump pattern), bishops (diagonals), rooks (ranks/files)
- **Queen moves**: Combination of rook + bishop movement
- **King moves**: Single square in any direction, plus castling with validation

Every move is validated against legal move rules: castling rights are tracked, checks are detected, and only moves that leave the king safe are returned to the search algorithm. Board state snapshots (stored in a `BoardState` struct) are maintained for each move in history, enabling efficient undo operations.

### AI Search: Minimax with Alpha-Beta Pruning
The engine uses the **Minimax algorithm** to recursively evaluate positions and select optimal moves. To improve speed, I implemented **alpha-beta pruning**, which eliminates branches of the game tree that cannot possibly affect the final decision. The search also includes a **quiescence search** extension to avoid the "horizon effect"—evaluating positions in the middle of captures and tactics rather than stopping at arbitrary depths.

## Challenges Encountered: Memory Management Refactoring

The most significant challenge I faced was memory management during the search phase. Initially, when the AI would try and find the best possible move I would save all the moves which had a chance to be the best move. As the search tree grew deeper (evaluating 5+ plies on a standard chess position), the move history array would exponentially accumulate moves, consuming GBs of memory for a single search. I quickly hit memory constraints that caused my laptop to crash.

I refactored the move storage architecture to decouple move data from board state history:

- **Move structure** was reduced to contain only the essential data: source square, destination square, piece type, capture type, and castle flags (totaling ~48 bytes per move instead of 150+)
- **BoardState snapshots** are now stored separately in MoveHistoryEntry structures, maintaining only one full board state per move in the history (enabling efficient undo)
- The search phase now uses temporary board state updates during alpha-beta exploration, only storing complete snapshots when moves are committed to the history

This refactoring reduced memory footprint by ~70%, allowing deeper searches while maintaining the ability to undo moves correctly. It reinforced the importance of understanding data structure efficiency and memory layout in performance-critical code, lessons directly applicable to embedded systems where memory is constrained.

## Learning & Motivation

This project stemmed from a genuine passion to build something engaging while learning fundamental computer science concepts. Rather than following a tutorial or using an existing engine, I chose to implement bitboards and search algorithms from scratch. I spent time with Wikipedia articles on chess algorithms, bitboard techniques, and minimax search, translating those concepts into working C code.

The decision to use bitboards specifically came from my interest in understanding **where bitwise operations are actually used in real software**. Rather than studying bitshifts in isolation, I wanted to see them solve a real problem—and in a chess engine, bitboards showcase how low-level bit manipulation directly translates to speed and efficiency. This hands-on learning experience taught me far more than any tutorial could have.

## Technical Outcomes & Current Status

The engine is now capable of:
- Generating and validating legal moves for all piece types
- Detecting check, checkmate, and stalemate conditions  
- Playing against human opponents with configurable search depth
- Running with reasonable move latency (sub-second decisions at depth 4-5 on standard hardware)

The project includes a C++ frontend for user interaction and input handling, integrated with the engine via a C interface layer. Code is organized into header files with clear separation of concerns: `moves.h` for move generation lookup tables, `globals.h` for shared structures, and `main.c` for core engine logic.

## Relevance to Your Role

This project demonstrates skills highly relevant to embedded firmware development at Zaber:
- **Deep understanding of performance tradeoffs**: Memory vs. speed, algorithmic complexity
- **Problem-solving under constraints**: Refactoring to fit memory budgets
- **Systematic debugging**: Tracking state changes, validating correctness through move undo/redo
- **Attention to low-level systems**: Bitwise operations, data structure layout, efficient algorithms
- **Self-directed learning**: Understanding concepts from references, then implementing them correctly
- **Code quality practices**: Modular design, clear data structures, comprehensive move validation

I'm excited to bring this same problem-solving mindset and attention to detail to embedded systems work, where these principles directly impact real-time control and resource efficiency.
