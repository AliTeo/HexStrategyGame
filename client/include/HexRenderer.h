#pragma once

#include <SFML/Graphics.hpp>
#include "GameState.h"
#include "HexCoord.h"
#include <memory>
#include <optional>
#include <string>
#include <vector>

class HexRenderer {
public:
    HexRenderer();
    
    void setHexSize(float size);
    void setGridCenter(sf::Vector2f center);
    
    // Drawing functions
    void drawHexGrid(sf::RenderTarget& window, const GameState& state, int myPlayerId);
    void drawCharacter(sf::RenderTarget& window, const Character& character, const HexCoord& pos, int myPlayerId);
    void drawUI(sf::RenderTarget& window,
                const GameState& state,
                int myPlayerId,
                const std::vector<std::string>& infoLines);
    void drawSelectionScreen(sf::RenderTarget& window,
                             const std::vector<CharacterClass>& selected,
                             std::optional<CharacterClass> hoveredClass,
                             bool readyEnabled,
                             bool readySent,
                             bool readyHovered,
                             float elapsedSeconds,
                             std::optional<CharacterClass> justSelectedClass,
                             bool opponentReady);
    void drawPlacementScreen(sf::RenderTarget& window, const GameState& state, int myPlayerId);
    void drawPendingActions(sf::RenderTarget& window, const GameState& state, 
                            const std::vector<Action>& actions);
    void drawSelectionHighlight(sf::RenderTarget& window, const HexCoord& hex);
    void drawRangeOverlay(sf::RenderTarget& window,
                          const HexCoord& center,
                          int moveRange,
                          int attackRange,
                          bool showMove,
                          bool showAttack);
    
    // Utility functions
    sf::Vector2f hexToPixel(const HexCoord& hex) const;
    HexCoord pixelToHex(sf::Vector2f pixel) const;
    HexCoord getHexUnderMouse(sf::RenderWindow& window) const;
    std::optional<CharacterClass> getSelectionClassAt(sf::Vector2f point) const;
    bool isSelectionReadyButtonAt(sf::Vector2f point) const;
    
private:
    void drawHexagon(sf::RenderTarget& window, sf::Vector2f center, sf::Color fillColor, sf::Color outlineColor);
    void drawClassGlyph(sf::RenderTarget& window, CharacterClass charClass, sf::Vector2f center, float scale, sf::Color color);
    void drawHealthBar(sf::RenderTarget& window, sf::Vector2f pos, int current, int max);
    void drawArrow(sf::RenderTarget& window, sf::Vector2f from, sf::Vector2f to, sf::Color color);
    std::string getCharacterClassSymbol(CharacterClass charClass) const;
    std::string getClassName(CharacterClass charClass) const;
    sf::Color getPlayerColor(int playerId) const;
    sf::Color getClassColor(CharacterClass charClass) const;
    
    float hexSize_;
    sf::Vector2f gridCenter_;
    sf::Font font_;
    bool fontLoaded_;
};
