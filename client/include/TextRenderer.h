#pragma once

#include "GameState.h"
#include <string>
#include <iostream>

/**
 * Text-based renderer for console output
 */
class TextRenderer {
public:
    TextRenderer();
    
    // Display methods
    void displayWelcome();
    void displayLogo();
    void displayGameState(const GameState& state, int myPlayerId);
    void displayHexGrid(const GameState& state, int myPlayerId);
    void displayCharacterList(const GameState& state, int myPlayerId);
    void displayAvailableClasses();
    void displayTurnSummary(const std::string& summary);
    void displayError(const std::string& error);
    void displayMessage(const std::string& message);
    void displayHelp();
    void displayPrompt(const std::string& prompt);
    
    // Utility
    void clearScreen();
    void waitForInput();
    
private:
    void displayHexRow(const GameState& state, int r, int qMin, int qMax);
    std::string getCharacterSymbol(const Character& character) const;
    std::string getTerrainSymbol(TerrainType terrain) const;
};
