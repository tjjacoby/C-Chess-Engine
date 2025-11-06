//TODO

/*
    Idea: Create a GUI using C++ that interfaces with the chess engine.
    This GUI would allow users to play against the engine, visualize the board.
    Thinking of using SFML for GUI development.

    Steps to implement:
    1. Set up SFML in the project.
    2. Design the chessboard layout and pieces.
    3. Implement user input handling for piece movement. (and send move to engine)
    4. Connect the GUI with the existing chess engine logic.

*/

//First step would be setting up SFML and creating a window with a chessboard displayed.
#include <SFML/Graphics.hpp>
#include "engine_interface.h"  // Include the chess engine interface
#include <iostream>
#include <map>
#include <string>

// Piece types: wp=white pawn, wr=white rook, etc. Empty square = ""
std::string board[8][8];

// Helper function to convert bitboard to board array position
void bitboardToSquare(uint64_t bitboard, int& row, int& col) {
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            uint64_t square = engine_squareFromRowCol(r, c);
            if (bitboard == square) {
                row = r;
                col = c;
                return;
            }
        }
    }
    row = -1;
    col = -1;
}

void syncBoardFromEngine() {
    // Clear the board
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            board[i][j] = "";
        }
    }
    
    // Get current state from engine
    uint64_t wp, wn, wb, wr, wq, wk, bp, bn, bb, br, bq, bk;
    engine_getBoardState(&wp, &wn, &wb, &wr, &wq, &wk, &bp, &bn, &bb, &br, &bq, &bk);
    
    // Place white pieces
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            uint64_t square = engine_squareFromRowCol(r, c);
            
            if (wp & square) board[r][c] = "wp";
            else if (wn & square) board[r][c] = "wn";
            else if (wb & square) board[r][c] = "wb";
            else if (wr & square) board[r][c] = "wr";
            else if (wq & square) board[r][c] = "wq";
            else if (wk & square) board[r][c] = "wk";
            else if (bp & square) board[r][c] = "bp";
            else if (bn & square) board[r][c] = "bn";
            else if (bb & square) board[r][c] = "bb";
            else if (br & square) board[r][c] = "br";
            else if (bq & square) board[r][c] = "bq";
            else if (bk & square) board[r][c] = "bk";
        }
    }
}

int main() {
    // Initialize the chess engine
    engine_init();
    engine_reset();
    // Create the main window (800x800 pixels)
    sf::RenderWindow window(sf::VideoMode(800, 800), "Chess Engine GUI");
    
    // Optional: Set frame rate limit to avoid excessive CPU usage
    window.setFramerateLimit(60);

    std::cout << "Window created successfully!" << std::endl;

    // Sync initial board state from engine
    syncBoardFromEngine();
    
    std::cout << "Player is White (bottom). AI is Black (top). Make your move!" << std::endl;

    // Load piece textures
    std::map<std::string, sf::Texture> textures;
    
    // Load white pieces
    textures["wp"].loadFromFile("IMG_Pieces/white-pawn.png");
    textures["wr"].loadFromFile("IMG_Pieces/white-rook.png");
    textures["wn"].loadFromFile("IMG_Pieces/white-knight.png");
    textures["wb"].loadFromFile("IMG_Pieces/white-bishop.png");
    textures["wq"].loadFromFile("IMG_Pieces/white-queen.png");
    textures["wk"].loadFromFile("IMG_Pieces/white-king.png");
    
    // Load black pieces
    textures["bp"].loadFromFile("IMG_Pieces/black-pawn.png");
    textures["br"].loadFromFile("IMG_Pieces/black-rook.png");
    textures["bn"].loadFromFile("IMG_Pieces/black-knight.png");
    textures["bb"].loadFromFile("IMG_Pieces/black-bishop.png");
    textures["bq"].loadFromFile("IMG_Pieces/black-queen.png");
    textures["bk"].loadFromFile("IMG_Pieces/black-king.png");

    // Drag and drop state
    bool isDragging = false;
    bool waitingForAI = false;
    int dragFromRow = -1, dragFromCol = -1;
    std::string draggedPiece = "";
    sf::Vector2f mousePos;
    
    // Game state
    bool playerTurn = true;  // Player (white) starts

    // Main game loop - keeps the window open
    float squareSize = 100.0f; // Each square is 100x100 pixels
    
    while (window.isOpen()) {
        // Event handling - check for user input
        sf::Event event;
        while (window.pollEvent(event)) {
            // Close window when user clicks the X button
            if (event.type == sf::Event::Closed)
                window.close();
            
            // Optional: Close on Escape key press
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape)
                    window.close();
            }
            
            // Mouse button pressed - start dragging
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left && playerTurn && !waitingForAI) {
                    int col = event.mouseButton.x / squareSize;
                    int row = event.mouseButton.y / squareSize;
                    
                    // Check if there's a white piece at this position (player is white)
                    if (row >= 0 && row < 8 && col >= 0 && col < 8 && board[row][col] != "") {
                        // Only allow white pieces
                        if (board[row][col][0] == 'w') {
                            isDragging = true;
                            dragFromRow = row;
                            dragFromCol = col;
                            draggedPiece = board[row][col];
                            board[row][col] = ""; // Temporarily remove from board
                            std::cout << "Picked up " << draggedPiece << " from (" << row << ", " << col << ")" << std::endl;
                        } else {
                            std::cout << "That's not your piece! You are white (bottom)." << std::endl;
                        }
                    }
                }
            }
            
            // Mouse button released - drop piece
            if (event.type == sf::Event::MouseButtonReleased) {
                if (event.mouseButton.button == sf::Mouse::Left && isDragging) {
                    int col = event.mouseButton.x / squareSize;
                    int row = event.mouseButton.y / squareSize;
                    
                    // Check if drop location is valid
                    if (row >= 0 && row < 8 && col >= 0 && col < 8) {
                        // Try to make the move in the engine
                        uint64_t fromSquare = engine_squareFromRowCol(dragFromRow, dragFromCol);
                        uint64_t toSquare = engine_squareFromRowCol(row, col);
                        
                        int moveValid = engine_makePlayerMove(fromSquare, toSquare);
                        
                        if (moveValid) {
                            std::cout << "Valid move! Waiting for AI..." << std::endl;
                            syncBoardFromEngine();  // Update board from engine
                            playerTurn = false;
                            waitingForAI = true;
                        } else {
                            std::cout << "Invalid move! Returning piece to original position." << std::endl;
                            board[dragFromRow][dragFromCol] = draggedPiece;
                        }
                    } else {
                        // Return to original position if dropped outside board
                        board[dragFromRow][dragFromCol] = draggedPiece;
                        std::cout << "Invalid drop, returned to original position" << std::endl;
                    }
                    
                    isDragging = false;
                    draggedPiece = "";
                }
            }
        }
        
        // Update mouse position for dragging
        if (isDragging) {
            sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
            mousePos = window.mapPixelToCoords(pixelPos);
        }
        
        // AI makes its move
        if (waitingForAI) {
            SimpleMove aiMove = engine_getAIMove();
            
            if (aiMove.from != 0 || aiMove.to != 0) {
                syncBoardFromEngine();  // Update board from engine
                std::cout << "AI has moved. Your turn!" << std::endl;
            } else {
                std::cout << "AI has no moves - Game Over!" << std::endl;
            }
            
            waitingForAI = false;
            playerTurn = true;
        }

        // Clear the window with a color (try different colors!)
        window.clear(sf::Color(50, 50, 50));  // Dark gray background

        //Loop to add black and white squares for chessboard
        //8x8 grid starting with white square top left
        for(int row = 0; row < 8; ++row) {
            for(int col = 0; col < 8; ++col) {
                sf::RectangleShape square(sf::Vector2f(squareSize, squareSize));
                square.setPosition(col * squareSize, row * squareSize);
                
                // Alternate colors
                if((row + col) % 2 == 0) {
                    square.setFillColor(sf::Color(240, 217, 181)); // Light square
                } else {
                    square.setFillColor(sf::Color(181, 136, 99)); // Dark square
                }

                window.draw(square);
            }
        }

        // Draw all pieces on the board (except the one being dragged)
        for(int row = 0; row < 8; ++row) {
            for(int col = 0; col < 8; ++col) {
                std::string piece = board[row][col];
                if (piece != "" && textures.find(piece) != textures.end()) {
                    sf::Sprite sprite;
                    sprite.setTexture(textures[piece]);
                    
                    // Scale the sprite to fit the square
                    float scale = squareSize / textures[piece].getSize().x;
                    sprite.setScale(scale, scale);
                    
                    sprite.setPosition(col * squareSize, row * squareSize);
                    window.draw(sprite);
                }
            }
        }
        
        // Draw the dragged piece following the mouse
        if (isDragging && textures.find(draggedPiece) != textures.end()) {
            sf::Sprite sprite;
            sprite.setTexture(textures[draggedPiece]);
            
            // Scale the sprite
            float scale = squareSize / textures[draggedPiece].getSize().x;
            sprite.setScale(scale, scale);
            
            // Center the sprite on the mouse cursor
            sprite.setPosition(mousePos.x - squareSize / 2, mousePos.y - squareSize / 2);
            
            // Make it slightly transparent while dragging
            sprite.setColor(sf::Color(255, 255, 255, 200));
            
            window.draw(sprite);
        }
        
        // Display what we've drawn to the window
        window.display();
    }

    std::cout << "Window closed." << std::endl;
    return 0;
}
