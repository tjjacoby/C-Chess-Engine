# C Chess Engine

A chess engine built in C using bitboard representation and advanced search algorithms.

## Overview

This chess engine uses **bitboards** (64-bit integers) to represent the chess board and legal moves efficiently. The engine leverages bitwise operations for fast move generation and employs a **Minimax algorithm with alpha-beta pruning** to find optimal moves for the AI.

## Features

- **Bitboard Representation**: Each piece type and board position is represented using 64-bit integers for efficient storage and manipulation
- **Bitwise Operations**: Fast move generation using bitwise operations (AND, OR, XOR, shifts)
- **Minimax Algorithm**: Game tree search to evaluate positions and find the best move
- **Alpha-Beta Pruning**: Optimization technique to reduce the number of nodes evaluated in the search tree
- **Legal Move Generation**: Validates and generates all legal moves according to chess rules

## Technical Details

The engine uses several key components:
- `moves.h`: Defines all 64 board squares as bitboard constants and move generation utilities
- `binary_printer.h`: Debugging utilities for visualizing bitboards
- `globals.h`: Shared data structures and function prototypes
- `main.c`: Core engine logic, move generation, and AI search algorithm

## Building and Running

### Prerequisites
- GCC compiler
- Make utility

### Compilation

```bash
make
```

### Running the Engine

```bash
./main
```

### Clean Build

```bash
make clean
```

## Project Status

**Initial Development**: Late 2023 - Created working prototype with legal move generation and weak AI

**Current Phase**: Resuming development with the goal of creating an AI capable of defeating strong human players

## Future Goals

- [ ] Improve evaluation function for better position assessment
- [ ] Implement deeper search with iterative deepening
- [ ] Add opening book support
- [ ] Optimize move ordering for better pruning
- [ ] Implement endgame tablebases
- [ ] Add UCI (Universal Chess Interface) protocol support



## Author

Ty Jacoby
