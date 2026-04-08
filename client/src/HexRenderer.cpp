#include "HexRenderer.h"
#include <cmath>
#include <sstream>

HexRenderer::HexRenderer() 
    : hexSize_(40.0f), gridCenter_(400.0f, 300.0f), fontLoaded_(false) {
    // Try to load system font (will use default if not available)
    // In a real app, you'd ship a font file
}

void HexRenderer::setHexSize(float size) {
    hexSize_ = size;
}

void HexRenderer::setGridCenter(sf::Vector2f center) {
    gridCenter_ = center;
}

sf::Vector2f HexRenderer::hexToPixel(const HexCoord& hex) const {
    // Flat-top hexagon layout
    float x = hexSize_ * (3.0f/2.0f * hex.q);
    float y = hexSize_ * (std::sqrt(3.0f)/2.0f * hex.q + std::sqrt(3.0f) * hex.r);
    return gridCenter_ + sf::Vector2f(x, y);
}

HexCoord HexRenderer::pixelToHex(sf::Vector2f pixel) const {
    sf::Vector2f relative = pixel - gridCenter_;
    
    // Convert pixel to cube coordinates (approximation)
    float q = (2.0f/3.0f * relative.x) / hexSize_;
    float r = (-1.0f/3.0f * relative.x + std::sqrt(3.0f)/3.0f * relative.y) / hexSize_;
    
    // Round to nearest hex
    int qi = static_cast<int>(std::round(q));
    int ri = static_cast<int>(std::round(r));
    
    return HexCoord(qi, ri);
}

HexCoord HexRenderer::getHexUnderMouse(sf::RenderWindow& window) const {
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    return pixelToHex(sf::Vector2f(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)));
}

void HexRenderer::drawHexagon(sf::RenderWindow& window, sf::Vector2f center, 
                              sf::Color fillColor, sf::Color outlineColor) {
    sf::CircleShape hexagon(hexSize_ * 0.95f, 6);
    hexagon.setOrigin(sf::Vector2f(hexSize_ * 0.95f, hexSize_ * 0.95f));
    hexagon.setPosition(center);
    hexagon.setFillColor(fillColor);
    hexagon.setOutlineColor(outlineColor);
    hexagon.setOutlineThickness(2.0f);
    hexagon.setRotation(sf::degrees(30.0f));
    
    window.draw(hexagon);
}

sf::Color HexRenderer::getPlayerColor(int playerId) const {
    return playerId == 0 ? sf::Color(100, 150, 255) : sf::Color(255, 100, 100);
}

sf::Color HexRenderer::getClassColor(CharacterClass charClass) const {
    switch (charClass) {
        case CharacterClass::WARRIOR: return sf::Color(200, 100, 100);
        case CharacterClass::MAGE: return sf::Color(150, 100, 255);
        case CharacterClass::HEALER: return sf::Color(255, 255, 100);
        case CharacterClass::ROGUE: return sf::Color(150, 150, 150);
        case CharacterClass::TANK: return sf::Color(139, 69, 19);
        case CharacterClass::SUMMONER: return sf::Color(100, 200, 100);
        default: return sf::Color::White;
    }
}

std::string HexRenderer::getCharacterClassSymbol(CharacterClass charClass) const {
    switch (charClass) {
        case CharacterClass::WARRIOR: return "W";
        case CharacterClass::MAGE: return "M";
        case CharacterClass::HEALER: return "H";
        case CharacterClass::ROGUE: return "R";
        case CharacterClass::TANK: return "T";
        case CharacterClass::SUMMONER: return "S";
        default: return "?";
    }
}

void HexRenderer::drawHexGrid(sf::RenderWindow& window, const GameState& state, int myPlayerId) {
    // Draw grid hexes
    for (int q = -GameState::GRID_RADIUS; q <= GameState::GRID_RADIUS; ++q) {
        for (int r = -GameState::GRID_RADIUS; r <= GameState::GRID_RADIUS; ++r) {
            HexCoord hex(q, r);
            if (!hex.isValid(GameState::GRID_RADIUS)) continue;
            
            sf::Vector2f pos = hexToPixel(hex);
            
            // Determine hex color based on ownership
            sf::Color fillColor = sf::Color(50, 50, 60);
            sf::Color outlineColor = sf::Color(100, 100, 120);
            
            if (q < 0) {
                fillColor = sf::Color(60, 60, 80); // Player 0 side
            } else if (q > 0) {
                fillColor = sf::Color(80, 60, 60); // Player 1 side
            }
            
            drawHexagon(window, pos, fillColor, outlineColor);
            
            // Draw character if present
            auto it = state.occupancy.find(hex);
            if (it != state.occupancy.end()) {
                auto character = state.getCharacter(it->second);
                if (character) {
                    drawCharacter(window, *character, hex);
                }
            }
        }
    }
}

void HexRenderer::drawCharacter(sf::RenderWindow& window, const Character& character, const HexCoord& pos) {
    sf::Vector2f pixelPos = hexToPixel(pos);
    
    // Draw character as colored circle with symbol
    sf::CircleShape charCircle(hexSize_ * 0.6f);
    charCircle.setOrigin(sf::Vector2f(hexSize_ * 0.6f, hexSize_ * 0.6f));
    charCircle.setPosition(pixelPos);
    charCircle.setFillColor(getClassColor(character.charClass));
    charCircle.setOutlineColor(getPlayerColor(character.playerId));
    charCircle.setOutlineThickness(3.0f);
    
    window.draw(charCircle);
    
    // Draw health bar
    drawHealthBar(window, sf::Vector2f(pixelPos.x, pixelPos.y - hexSize_ * 0.8f), 
                  character.stats.currentHp, character.stats.maxHp);
}

void HexRenderer::drawHealthBar(sf::RenderWindow& window, sf::Vector2f pos, int current, int max) {
    float barWidth = hexSize_ * 1.2f;
    float barHeight = 6.0f;
    
    // Background
    sf::RectangleShape bg(sf::Vector2f(barWidth, barHeight));
    bg.setPosition(sf::Vector2f(pos.x - barWidth/2, pos.y));
    bg.setFillColor(sf::Color(100, 0, 0));
    window.draw(bg);
    
    // Health
    float healthPercent = static_cast<float>(current) / max;
    sf::RectangleShape health(sf::Vector2f(barWidth * healthPercent, barHeight));
    health.setPosition(sf::Vector2f(pos.x - barWidth/2, pos.y));
    health.setFillColor(sf::Color(0, 255, 0));
    window.draw(health);
}

void HexRenderer::drawUI(sf::RenderWindow& window, const GameState& state, int myPlayerId) {
    (void)state;  // Suppress unused warning
    (void)myPlayerId;  // Suppress unused warning
    
    // Draw phase indicator
    sf::RectangleShape panel(sf::Vector2f(200.0f, 100.0f));
    panel.setPosition(sf::Vector2f(10.0f, 10.0f));
    panel.setFillColor(sf::Color(40, 40, 50, 200));
    panel.setOutlineColor(sf::Color::White);
    panel.setOutlineThickness(2.0f);
    window.draw(panel);
    
    // We'll add text rendering in the next version with proper font
}

void HexRenderer::drawSelectionScreen(sf::RenderWindow& window, 
                                      const std::vector<CharacterClass>& selected) {
    (void)selected;  // Suppress unused warning
    
    // Title
    sf::RectangleShape titleBg(sf::Vector2f(600.0f, 80.0f));
    titleBg.setPosition(sf::Vector2f(100.0f, 50.0f));
    titleBg.setFillColor(sf::Color(30, 30, 40));
    titleBg.setOutlineColor(sf::Color::White);
    titleBg.setOutlineThickness(2.0f);
    window.draw(titleBg);
    
    // Draw class selection boxes
    for (int i = 0; i < 6; ++i) {
        CharacterClass charClass = static_cast<CharacterClass>(i);
        
        float x = 100.0f + (i % 3) * 220.0f;
        float y = 200.0f + (i / 3) * 180.0f;
        
        sf::RectangleShape box(sf::Vector2f(200.0f, 150.0f));
        box.setPosition(sf::Vector2f(x, y));
        box.setFillColor(getClassColor(charClass));
        box.setOutlineColor(sf::Color::White);
        box.setOutlineThickness(3.0f);
        window.draw(box);
        
        // Draw circle for visual representation
        sf::CircleShape icon(40.0f);
        icon.setPosition(sf::Vector2f(x + 80.0f, y + 30.0f));
        icon.setFillColor(sf::Color::White);
        window.draw(icon);
    }
    
    // Draw selected count indicator
    sf::RectangleShape selectedPanel(sf::Vector2f(600.0f, 60.0f));
    selectedPanel.setPosition(sf::Vector2f(100.0f, 550.0f));
    selectedPanel.setFillColor(sf::Color(30, 30, 40));
    selectedPanel.setOutlineColor(sf::Color::White);
    selectedPanel.setOutlineThickness(2.0f);
    window.draw(selectedPanel);
}

void HexRenderer::drawPlacementScreen(sf::RenderWindow& window, 
                                      const GameState& state, int myPlayerId) {
    // Draw the hex grid
    drawHexGrid(window, state, myPlayerId);
    
    // Draw placement instructions panel
    sf::RectangleShape panel(sf::Vector2f(300.0f, 100.0f));
    panel.setPosition(sf::Vector2f(static_cast<float>(window.getSize().x) - 310.0f, 10.0f));
    panel.setFillColor(sf::Color(30, 30, 40, 200));
    panel.setOutlineColor(sf::Color::White);
    panel.setOutlineThickness(2.0f);
    window.draw(panel);
}

void HexRenderer::drawArrow(sf::RenderWindow& window, sf::Vector2f from, sf::Vector2f to, sf::Color color) {
    // Draw line using SFML 3.0 Vertex struct
    sf::Vertex line[2];
    line[0].position = from;
    line[0].color = color;
    line[1].position = to;
    line[1].color = color;
    window.draw(line, 2, sf::PrimitiveType::Lines);
    
    // Draw arrowhead
    sf::Vector2f direction = to - from;
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    if (length > 0) {
        direction /= length;
        
        // Perpendicular vector
        sf::Vector2f perpendicular(-direction.y, direction.x);
        
        float arrowSize = 15.0f;
        sf::Vector2f arrowTip = to;
        sf::Vector2f arrowLeft = to - direction * arrowSize + perpendicular * (arrowSize * 0.5f);
        sf::Vector2f arrowRight = to - direction * arrowSize - perpendicular * (arrowSize * 0.5f);
        
        sf::ConvexShape arrowhead;
        arrowhead.setPointCount(3);
        arrowhead.setPoint(0, arrowTip);
        arrowhead.setPoint(1, arrowLeft);
        arrowhead.setPoint(2, arrowRight);
        arrowhead.setFillColor(color);
        window.draw(arrowhead);
    }
}

void HexRenderer::drawPendingActions(sf::RenderWindow& window, const GameState& state, 
                                     const std::vector<Action>& actions) {
    for (const auto& action : actions) {
        // Find character position using occupancy map
        sf::Vector2f fromPos;
        bool foundChar = false;
        
        for (const auto& [pos, charId] : state.occupancy) {
            if (charId == action.characterId) {
                fromPos = hexToPixel(pos);
                foundChar = true;
                break;
            }
        }
        
        if (!foundChar) continue;
        
        sf::Vector2f toPos = hexToPixel(action.target);
        
        // Choose color based on action type
        sf::Color arrowColor;
        if (action.type == ActionType::MOVE) {
            arrowColor = sf::Color(100, 200, 255, 220);  // Blue for move
        } else if (action.type == ActionType::USE_SKILL) {
            arrowColor = sf::Color(255, 100, 100, 220);  // Red for attack
        } else {
            arrowColor = sf::Color(150, 150, 150, 220);  // Gray for other
        }
        
        drawArrow(window, fromPos, toPos, arrowColor);
    }
}

void HexRenderer::drawSelectionHighlight(sf::RenderWindow& window, const HexCoord& hex) {
    sf::Vector2f pos = hexToPixel(hex);
    
    // Draw pulsing highlight circle
    sf::CircleShape highlight(hexSize_ * 1.1f, 6);
    highlight.setOrigin(sf::Vector2f(hexSize_ * 1.1f, hexSize_ * 1.1f));
    highlight.setPosition(pos);
    highlight.setFillColor(sf::Color::Transparent);
    highlight.setOutlineColor(sf::Color(255, 255, 0, 180));  // Yellow highlight
    highlight.setOutlineThickness(4.0f);
    highlight.setRotation(sf::degrees(30.0f));
    
    window.draw(highlight);
}

