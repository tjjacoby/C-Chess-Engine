

//First step would be setting up SFML and creating a window with a chessboard displayed.
#include <SFML/Graphics.hpp>
#include "engine_interface.h"  // Include the chess engine interface
#include <iostream>
#include <map>
#include <string>

// Piece types: wp=white pawn, wr=white rook, etc. Empty square = ""
std::string board[8][8];
bool IsWhite = true; // Global variable to track if player is white
char playerPieceColor = 'w'; // Global variable to track player's piece color ('w' or 'b')

// Helper function to convert bitboard to board array position
void bitboardToSquare(uint64_t bitboard, int& row, int& col) {
    if (bitboard == 0) {
        row = -1;
        col = -1;
        return;
    }
    
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
   // std::cout << "DEBUG: Starting board sync..." << std::endl;
    
    // Clear the board
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            board[i][j] = "";
        }
    }
    
    // Get current state from engine
    uint64_t wp, wn, wb, wr, wq, wk, bp, bn, bb, br, bq, bk;
    engine_getBoardState(&wp, &wn, &wb, &wr, &wq, &wk, &bp, &bn, &bb, &br, &bq, &bk);
    
   // std::cout << "DEBUG: Got board state from engine" << std::endl;
    
    // Place pieces
    int pieceCount = 0;
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            uint64_t square = engine_squareFromRowCol(r, c);
            
            if (wp & square) { board[r][c] = "wp"; pieceCount++; }
            else if (wn & square) { board[r][c] = "wn"; pieceCount++; }
            else if (wb & square) { board[r][c] = "wb"; pieceCount++; }
            else if (wr & square) { board[r][c] = "wr"; pieceCount++; }
            else if (wq & square) { board[r][c] = "wq"; pieceCount++; }
            else if (wk & square) { board[r][c] = "wk"; pieceCount++; }
            else if (bp & square) { board[r][c] = "bp"; pieceCount++; }
            else if (bn & square) { board[r][c] = "bn"; pieceCount++; }
            else if (bb & square) { board[r][c] = "bb"; pieceCount++; }
            else if (br & square) { board[r][c] = "br"; pieceCount++; }
            else if (bq & square) { board[r][c] = "bq"; pieceCount++; }
            else if (bk & square) { board[r][c] = "bk"; pieceCount++; }
        }
    }
    
    //std::cout << "DEBUG: Board sync complete. Total pieces: " << pieceCount << std::endl;
}
int pick_colour() {
    // Create a small popup window for color selection
    sf::RenderWindow colorWindow(sf::VideoMode(400, 200), "Choose Your Color");
    colorWindow.setFramerateLimit(60);
    
    // Create two rectangles for the buttons
    sf::RectangleShape whiteButton(sf::Vector2f(150, 80));
    whiteButton.setPosition(50, 60);
    whiteButton.setFillColor(sf::Color(240, 240, 240));
    whiteButton.setOutlineThickness(2);
    whiteButton.setOutlineColor(sf::Color::Black);
    
    sf::RectangleShape blackButton(sf::Vector2f(150, 80));
    blackButton.setPosition(200, 60);
    blackButton.setFillColor(sf::Color(50, 50, 50));
    blackButton.setOutlineThickness(2);
    blackButton.setOutlineColor(sf::Color::Black);
    
    // Load font (using SFML's default rendering for now)
    sf::Font font;
    // Try to load a system font - if it fails, we'll create simple text
    bool fontLoaded = false;
    if (font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
        fontLoaded = true;
    }
    
    // Create text labels
    sf::Text titleText, whiteText, blackText;
    if (fontLoaded) {
        titleText.setFont(font);
        titleText.setString("Choose Your Color");
        titleText.setCharacterSize(24);
        titleText.setFillColor(sf::Color::Black);
        titleText.setPosition(100, 20);
        
        whiteText.setFont(font);
        whiteText.setString("White");
        whiteText.setCharacterSize(20);
        whiteText.setFillColor(sf::Color::Black);
        whiteText.setPosition(85, 90);
        
        blackText.setFont(font);
        blackText.setString("Black");
        blackText.setCharacterSize(20);
        blackText.setFillColor(sf::Color::White);
        blackText.setPosition(235, 90);
    }
    
    int choice = -1; // -1 = no choice, 0 = black, 1 = white
    
    while (colorWindow.isOpen() && choice == -1) {
        sf::Event event;
        while (colorWindow.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                colorWindow.close();
                choice = 1; // Default to white if window is closed
            }
            
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(colorWindow);
                    
                    // Check if white button was clicked
                    if (whiteButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        choice = 1;
                        std::cout << "Player chose White" << std::endl;
                    }
                    // Check if black button was clicked
                    else if (blackButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        choice = 0;
                        std::cout << "Player chose Black" << std::endl;
                    }
                }
            }
        }
        
        // Highlight buttons on hover
        sf::Vector2i mousePos = sf::Mouse::getPosition(colorWindow);
        if (whiteButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
            whiteButton.setOutlineColor(sf::Color::Blue);
            whiteButton.setOutlineThickness(3);
        } else {
            whiteButton.setOutlineColor(sf::Color::Black);
            whiteButton.setOutlineThickness(2);
        }
        
        if (blackButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
            blackButton.setOutlineColor(sf::Color::Blue);
            blackButton.setOutlineThickness(3);
        } else {
            blackButton.setOutlineColor(sf::Color::Black);
            blackButton.setOutlineThickness(2);
        }
        
        // Draw everything
        colorWindow.clear(sf::Color(200, 200, 200));
        colorWindow.draw(whiteButton);
        colorWindow.draw(blackButton);

        
        if (fontLoaded) {
            colorWindow.draw(titleText);
            colorWindow.draw(whiteText);
            colorWindow.draw(blackText);
        }
        
        colorWindow.display();
    }
    
    colorWindow.close();
    return choice;
}

int main() {
    // Initialize the chess engine
    //Add function to let player pick white or black side
    IsWhite = pick_colour();
    playerPieceColor = IsWhite ? 'w' : 'b';
    const char* playercolour = IsWhite ? "White" : "Black";
    
    //std::cout << "DEBUG: IsWhite = " << IsWhite << std::endl;
    //std::cout << "DEBUG: playerPieceColor = " << playerPieceColor << std::endl;

    engine_init();
    engine_setPlayerColor(IsWhite); // Tell the engine which color the player chose
    engine_reset();
    // Create the main window (1000x800 pixels - extra 200px for score bar)
    sf::RenderWindow window(sf::VideoMode(1000, 800), "Chess Engine GUI");
    
    // Optional: Set frame rate limit to avoid excessive CPU usage
    window.setFramerateLimit(60);

    std::cout << "Window created successfully!" << std::endl;

    // AI move tracking for highlighting
    int aiLastMoveFromRow = -1, aiLastMoveFromCol = -1;
    int aiLastMoveToRow = -1, aiLastMoveToCol = -1;

    // Sync initial board state from engine
    syncBoardFromEngine();
    
    std::cout << "Player is " << playercolour << "." << std::endl;
    
    // If player is Black, AI (White) needs to make the first move
    if (!IsWhite) {
        std::cout << "AI is making the first move..." << std::endl;
        SimpleMove aiMove = engine_getAIMove();
        if (aiMove.from != 0 || aiMove.to != 0) {
            // Track AI's first move for highlighting
            bitboardToSquare(aiMove.from, aiLastMoveFromRow, aiLastMoveFromCol);
            bitboardToSquare(aiMove.to, aiLastMoveToRow, aiLastMoveToCol);
            
            syncBoardFromEngine();
            std::cout << "AI has moved. Your turn!" << std::endl;
        }
    } else {
        std::cout << "Make your move!" << std::endl;
    }

    // Load piece textures ONCE before loop
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
    
    // Load font ONCE before loop
    sf::Font font;
    bool fontLoaded = font.loadFromFile("C:/Windows/Fonts/arial.ttf");
    if (!fontLoaded) {
        std::cout << "Warning: Could not load font. Text will not be displayed." << std::endl;
    }
    
    // Pre-create reusable UI objects (allocated once, reused every frame)
    sf::RectangleShape blackBar, whiteBar, boardSquare;
    sf::Sprite pieceSprite, draggedSprite;
    sf::Text scoreText, blackLabel, whiteLabel, thinkingText;
    
    // Setup text objects once
    if (fontLoaded) {
        scoreText.setFont(font);
        scoreText.setCharacterSize(20);
        scoreText.setFillColor(sf::Color::White);
        
        blackLabel.setFont(font);
        blackLabel.setString("Black");
        blackLabel.setCharacterSize(18);
        blackLabel.setFillColor(sf::Color::White);
        
        whiteLabel.setFont(font);
        whiteLabel.setString("White");
        whiteLabel.setCharacterSize(18);
        whiteLabel.setFillColor(sf::Color::White);
        
        thinkingText.setFont(font);
        thinkingText.setString("AI is thinking...");
        thinkingText.setCharacterSize(22);
        thinkingText.setFillColor(sf::Color(255, 200, 100));
        thinkingText.setStyle(sf::Text::Bold);
    }

    // Drag and drop state
    bool isDragging = false;
    bool waitingForAI = false;
    int dragFromRow = -1, dragFromCol = -1;
    std::string draggedPiece = "";
    sf::Vector2f mousePos;
    
    // Game state - player always starts with their turn (after AI moves first if player is black)
    bool playerTurn = true;  

    //std::cout << "DEBUG: Game starting. playerTurn = " << playerTurn 
           //   << ", waitingForAI = " << waitingForAI << std::endl;

    // Main game loop - keeps the window open
    float squareSize = 100.0f; // Each square is 100x100 pixels
    
    while (window.isOpen()) {
        // AI makes its move at the START of the loop if needed
        // This ensures state is consistent before rendering
        if (waitingForAI) {
            try {
                SimpleMove aiMove = engine_getAIMove();
                
                if (aiMove.from != 0 || aiMove.to != 0) {
                    // Track AI's move for highlighting
                    bitboardToSquare(aiMove.from, aiLastMoveFromRow, aiLastMoveFromCol);
                    bitboardToSquare(aiMove.to, aiLastMoveToRow, aiLastMoveToCol);
                    
                    std::cout << "Syncing board from engine after AI move..." << std::endl;
                    syncBoardFromEngine();  // Update board from engine
                    std::cout << "Board synced successfully. Your turn!" << std::endl;
                    std::cout << "AI moved from (" << aiLastMoveFromRow << "," << aiLastMoveFromCol 
                              << ") to (" << aiLastMoveToRow << "," << aiLastMoveToCol << ")" << std::endl;
                } else {
                    std::cout << "AI has no moves - Game Over!" << std::endl;
                }
                
                waitingForAI = false;
                playerTurn = true;
            } catch (const std::exception& e) {
                std::cout << "ERROR during AI move: " << e.what() << std::endl;
                waitingForAI = false;
                playerTurn = true;
            }
        }
        
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
                //std::cout << "DEBUG: Mouse clicked. playerTurn=" << playerTurn 
                  //        << ", waitingForAI=" << waitingForAI << std::endl;
                          
                if (event.mouseButton.button == sf::Mouse::Left && playerTurn && !waitingForAI) {
                    int col = event.mouseButton.x / squareSize;
                    int row = event.mouseButton.y / squareSize;
                    
                    // Check if there's a piece at this position
                    if (row >= 0 && row < 8 && col >= 0 && col < 8 && board[row][col] != "") {
                        std::string piece = board[row][col];
                        char pieceColor = piece[0];
                        
                        //std::cout << "DEBUG: Clicked piece: " << piece << ", color: " << pieceColor 
                        //          << ", playerPieceColor: " << playerPieceColor << std::endl;
                        
                        // Only allow player's pieces based on their chosen color
                        if (pieceColor == playerPieceColor) {
                            isDragging = true;
                            dragFromRow = row;
                            dragFromCol = col;
                            draggedPiece = board[row][col];
                            board[row][col] = ""; // Temporarily remove from board
                            std::cout << "Picked up " << draggedPiece << " from (" << row << ", " << col << ")" << std::endl;
                        } else {
                            std::cout << "That's not your piece! You are playing as " 
                                      << (playerPieceColor == 'w' ? "White" : "Black") << "." << std::endl;
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
                            
                            // Clear AI move highlight when player makes a move
                            aiLastMoveFromRow = -1;
                            aiLastMoveFromCol = -1;
                            aiLastMoveToRow = -1;
                            aiLastMoveToCol = -1;
                            
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

        // Clear the window with a color (try different colors!)
        window.clear(sf::Color(50, 50, 50));  // Dark gray background

        // Draw score bar on the right side (800-1000px)
        float barX = 820;
        float barWidth = 160;
        float barHeight = 600;
        float barY = 100;
        
        // Get current score from engine
        int score = engine_getCurrentScore();
        
        // Normalize score for display (-1000 to 1000 range, but cap at +/- 500 for better visualization)
        float maxScore = 500.0f;
        float normalizedScore = score / maxScore;
        if (normalizedScore > 1.0f) normalizedScore = 1.0f;
        if (normalizedScore < -1.0f) normalizedScore = -1.0f;
        
        // Calculate bar heights (positive score = white advantage)
        float whiteHeight = barHeight * (0.5f + normalizedScore * 0.5f);
        float blackHeight = barHeight - whiteHeight;
        
        // Reuse blackBar object (update and draw)
        blackBar.setSize(sf::Vector2f(barWidth, blackHeight));
        blackBar.setPosition(barX, barY);
        blackBar.setFillColor(sf::Color(40, 40, 40));
        blackBar.setOutlineThickness(2);
        blackBar.setOutlineColor(sf::Color::Black);
        window.draw(blackBar);
        
        // Reuse whiteBar object (update and draw)
        whiteBar.setSize(sf::Vector2f(barWidth, whiteHeight));
        whiteBar.setPosition(barX, barY + blackHeight);
        whiteBar.setFillColor(sf::Color(240, 240, 240));
        whiteBar.setOutlineThickness(2);
        whiteBar.setOutlineColor(sf::Color::Black);
        window.draw(whiteBar);
        
        // Draw score text and status (reuse pre-allocated text objects)
        if (fontLoaded) {
            // Reuse scoreText - only update the string and position
            scoreText.setString("Score: " + std::to_string(score));
            scoreText.setPosition(barX + 10, barY + barHeight + 20);
            window.draw(scoreText);
            
            // Reuse blackLabel - only update position
            blackLabel.setPosition(barX + 50, barY - 30);
            window.draw(blackLabel);
            
            // Reuse whiteLabel - only update position
            whiteLabel.setPosition(barX + 50, barY + barHeight + 50);
            window.draw(whiteLabel);
            
            // Draw "AI is thinking..." message when waiting (reuse thinkingText)
            if (waitingForAI) {
                thinkingText.setPosition(barX + 20, 30);
                window.draw(thinkingText);
            }
        }

        //Loop to add black and white squares for chessboard
        //8x8 grid starting with white square top left
        // Reuse boardSquare object - set size once outside loop
        boardSquare.setSize(sf::Vector2f(squareSize, squareSize));
        
        for(int row = 0; row < 8; ++row) {
            for(int col = 0; col < 8; ++col) {
                // Reuse boardSquare - only update position and color
                boardSquare.setPosition(col * squareSize, row * squareSize);
                
                // Check if this is the AI's last move FROM square
                bool isAiFromSquare = (row == aiLastMoveFromRow && col == aiLastMoveFromCol);
                bool isAiToSquare = (row == aiLastMoveToRow && col == aiLastMoveToCol);
                
                // Alternate colors with highlight for AI's move
                if (isAiFromSquare) {
                    // Red tint for where AI moved from
                    boardSquare.setFillColor(sf::Color(220, 100, 100)); // Reddish
                } else if (isAiToSquare) {
                    // Yellow tint for where AI moved to
                    boardSquare.setFillColor(sf::Color(220, 220, 100)); // Yellowish
                } else if((row + col) % 2 == 0) {
                    boardSquare.setFillColor(sf::Color(240, 217, 181)); // Light square
                } else {
                    boardSquare.setFillColor(sf::Color(181, 136, 99)); // Dark square
                }

                window.draw(boardSquare);
            }
        }

        // Draw all pieces on the board (except the one being dragged)
        // Reuse pieceSprite object
        for(int row = 0; row < 8; ++row) {
            for(int col = 0; col < 8; ++col) {
                std::string piece = board[row][col];
                if (piece != "" && textures.find(piece) != textures.end()) {
                    try {
                        // Reuse pieceSprite - update texture, scale, and position
                        pieceSprite.setTexture(textures[piece]);
                        
                        // Scale the sprite to fit the square
                        float scale = squareSize / textures[piece].getSize().x;
                        pieceSprite.setScale(scale, scale);
                        
                        // Reset color to opaque (in case it was modified)
                        pieceSprite.setColor(sf::Color(255, 255, 255, 255));
                        
                        pieceSprite.setPosition(col * squareSize, row * squareSize);
                        window.draw(pieceSprite);
                    } catch (const std::exception& e) {
                        std::cout << "Error drawing piece '" << piece << "' at (" << row << "," << col << "): " << e.what() << std::endl;
                    }
                } else if (piece != "") {
                    std::cout << "Warning: Unknown piece type '" << piece << "' at (" << row << "," << col << ")" << std::endl;
                }
            }
        }
        
        // Draw the dragged piece following the mouse
        // Reuse draggedSprite object
        if (isDragging && textures.find(draggedPiece) != textures.end()) {
            draggedSprite.setTexture(textures[draggedPiece]);
            
            // Scale the sprite
            float scale = squareSize / textures[draggedPiece].getSize().x;
            draggedSprite.setScale(scale, scale);
            
            // Center the sprite on the mouse cursor
            draggedSprite.setPosition(mousePos.x - squareSize / 2, mousePos.y - squareSize / 2);
            
            // Make it slightly transparent while dragging
            draggedSprite.setColor(sf::Color(255, 255, 255, 200));
            
            window.draw(draggedSprite);
        }
        
        // Display what we've drawn to the window
        window.display();
    }

    std::cout << "Window closed." << std::endl;
    return 0;
}
