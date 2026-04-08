#pragma once

#include <SFML/Graphics.hpp>
#include "GameState.h"
#include "HexCoord.h"
#include <memory>
#include <string>

class HexRenderer {
public:
    HexRenderer();
    
    void setHexSize(float size);
    void setGridCenter(sf::Vector2f center);
    
    // Drawing functions
    void drawHexGrid(sf::RenderWindow& window, const GameState& state, int myPlayerId);
    void drawCharacter(sf::RenderWindow& window, const Character& character, const HexCoord& pos);
    void drawUI(sf::RenderWindow& window, const GameState& state, int myPlayerId);
    void drawSelectionScreen(sf::RenderWindow& window, const std::vector<CharacterClass>& selected);
    void drawPlacementScreen(sf::RenderWindow& window, const GameState& state, int myPlayerId);
    void drawPendingActions(sf::RenderWindow& window, const GameState& state, 
                            const std::vector<Action>& actions);
    void drawSelectionHighlight(sf::RenderWindow& window, const HexCoord& hex);
    
    // Utility functions
    sf::Vector2f hexToPixel(const HexCoord& hex) const;
    HexCoord pixelToHex(sf::Vector2f pixel) const;
    HexCoord getHexUnderMouse(sf::RenderWindow& window) const;
    
private:
    void drawHexagon(sf::RenderWindow& window, sf::Vector2f center, sf::Color fillColor, sf::Color outlineColor);
    void drawHealthBar(sf::RenderWindow& window, sf::Vector2f pos, int current, int max);
    void drawArrow(sf::RenderWindow& window, sf::Vector2f from, sf::Vector2f to, sf::Color color);
    std::string getCharacterClassSymbol(CharacterClass charClass) const;
    sf::Color getPlayerColor(int playerId) const;
    sf::Color getClassColor(CharacterClass charClass) const;
    
    float hexSize_;
    sf::Vector2f gridCenter_;
    sf::Font font_;
    bool fontLoaded_;
};
