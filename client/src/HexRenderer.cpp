#include "HexRenderer.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <sstream>

HexRenderer::HexRenderer() 
    : hexSize_(40.0f), gridCenter_(400.0f, 300.0f), fontLoaded_(false) {
    const std::array<const char*, 7> fontCandidates{
        "C:/Windows/Fonts/arial.ttf",
        "C:/Windows/Fonts/segoeui.ttf",
        "C:/Windows/Fonts/tahoma.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
        "/usr/share/fonts/TTF/DejaVuSans.ttf",
        "/System/Library/Fonts/Supplemental/Arial Unicode.ttf"
    };

    for (const char* path : fontCandidates) {
        if (font_.openFromFile(path)) {
            fontLoaded_ = true;
            break;
        }
    }
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

void HexRenderer::drawHexagon(sf::RenderTarget& window, sf::Vector2f center, 
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

void HexRenderer::drawClassGlyph(sf::RenderTarget& window,
                                 CharacterClass charClass,
                                 sf::Vector2f center,
                                 float scale,
                                 sf::Color color) {
    switch (charClass) {
        case CharacterClass::WARRIOR: {
            sf::ConvexShape triangle;
            triangle.setPointCount(3);
            triangle.setPoint(0, {center.x, center.y - 20.0f * scale});
            triangle.setPoint(1, {center.x - 18.0f * scale, center.y + 15.0f * scale});
            triangle.setPoint(2, {center.x + 18.0f * scale, center.y + 15.0f * scale});
            triangle.setFillColor(color);
            window.draw(triangle);
            break;
        }
        case CharacterClass::MAGE: {
            sf::CircleShape circle(18.0f * scale);
            circle.setOrigin({18.0f * scale, 18.0f * scale});
            circle.setPosition(center);
            circle.setFillColor(color);
            window.draw(circle);
            break;
        }
        case CharacterClass::HEALER: {
            sf::RectangleShape vBar({11.0f * scale, 40.0f * scale});
            vBar.setOrigin({5.5f * scale, 20.0f * scale});
            vBar.setPosition(center);
            vBar.setFillColor(color);
            window.draw(vBar);
            sf::RectangleShape hBar({40.0f * scale, 11.0f * scale});
            hBar.setOrigin({20.0f * scale, 5.5f * scale});
            hBar.setPosition(center);
            hBar.setFillColor(color);
            window.draw(hBar);
            break;
        }
        case CharacterClass::ROGUE: {
            sf::ConvexShape diamond;
            diamond.setPointCount(4);
            diamond.setPoint(0, {center.x, center.y - 21.0f * scale});
            diamond.setPoint(1, {center.x + 17.0f * scale, center.y});
            diamond.setPoint(2, {center.x, center.y + 21.0f * scale});
            diamond.setPoint(3, {center.x - 17.0f * scale, center.y});
            diamond.setFillColor(color);
            window.draw(diamond);
            break;
        }
        case CharacterClass::TANK: {
            sf::RectangleShape square({37.0f * scale, 37.0f * scale});
            square.setOrigin({18.5f * scale, 18.5f * scale});
            square.setPosition(center);
            square.setFillColor(color);
            window.draw(square);
            break;
        }
        case CharacterClass::SUMMONER: {
            sf::CircleShape hex(20.0f * scale, 6);
            hex.setOrigin({20.0f * scale, 20.0f * scale});
            hex.setPosition(center);
            hex.setRotation(sf::degrees(30.0f));
            hex.setFillColor(color);
            window.draw(hex);
            break;
        }
        default:
            break;
    }
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

std::string HexRenderer::getClassName(CharacterClass charClass) const {
    switch (charClass) {
        case CharacterClass::WARRIOR: return "Warrior";
        case CharacterClass::MAGE: return "Mage";
        case CharacterClass::HEALER: return "Healer";
        case CharacterClass::ROGUE: return "Rogue";
        case CharacterClass::TANK: return "Tank";
        case CharacterClass::SUMMONER: return "Summoner";
        default: return "Unknown";
    }
}

void HexRenderer::drawHexGrid(sf::RenderTarget& window, const GameState& state, int myPlayerId) {
    (void)myPlayerId;
    // Draw grid hexes
    for (int q = -GameState::GRID_RADIUS; q <= GameState::GRID_RADIUS; ++q) {
        for (int r = -GameState::GRID_RADIUS; r <= GameState::GRID_RADIUS; ++r) {
            HexCoord hex(q, r);
            if (!hex.isValid(GameState::GRID_RADIUS)) continue;
            
            sf::Vector2f pos = hexToPixel(hex);
            
            const int wave = (q - r + GameState::GRID_RADIUS * 2) % 3;
            sf::Color fillColor(48, 52, 66);
            sf::Color outlineColor(110, 120, 146);

            if (q < 0) {
                fillColor = sf::Color(54 + wave * 6, 66 + wave * 6, 96 + wave * 5);
            } else if (q > 0) {
                fillColor = sf::Color(95 + wave * 5, 64 + wave * 5, 64 + wave * 5);
            } else {
                fillColor = sf::Color(74 + wave * 4, 74 + wave * 4, 82 + wave * 4);
            }
            
            drawHexagon(window, pos, fillColor, outlineColor);
            
            // Draw character if present
            auto it = state.occupancy.find(hex);
            if (it != state.occupancy.end()) {
                auto character = state.getCharacter(it->second);
                if (character) {
                    drawCharacter(window, *character, hex, myPlayerId);
                }
            }
        }
    }
}

void HexRenderer::drawCharacter(sf::RenderTarget& window, const Character& character, const HexCoord& pos, int myPlayerId) {
    sf::Vector2f pixelPos = hexToPixel(pos);

    // Player ownership is represented by a class-shaped outline
    sf::Color ownerColor = getPlayerColor(character.playerId);
    if (myPlayerId >= 0) {
        ownerColor = (character.playerId == myPlayerId) ? sf::Color(96, 174, 255) : sf::Color(255, 118, 118);
    }
    drawClassGlyph(window, character.charClass, pixelPos, 1.55f, ownerColor);
    drawClassGlyph(window, character.charClass, pixelPos, 1.12f, getClassColor(character.charClass));
    
    // Draw health bar
    drawHealthBar(window, sf::Vector2f(pixelPos.x, pixelPos.y - hexSize_ * 0.8f), 
                  character.stats.currentHp, character.stats.maxHp);

    if (fontLoaded_) {
        sf::Text classText(font_, getClassName(character.charClass), 12);
        classText.setFillColor(sf::Color::White);
        const sf::FloatRect b = classText.getLocalBounds();
        classText.setPosition({pixelPos.x - b.size.x * 0.5f - b.position.x, pixelPos.y + hexSize_ * 0.56f});
        window.draw(classText);
    }
}

void HexRenderer::drawHealthBar(sf::RenderTarget& window, sf::Vector2f pos, int current, int max) {
    float barWidth = hexSize_ * 1.25f;
    float barHeight = 7.0f;
    
    // Background
    sf::RectangleShape bg(sf::Vector2f(barWidth, barHeight));
    bg.setPosition(sf::Vector2f(pos.x - barWidth/2, pos.y));
    bg.setFillColor(sf::Color(66, 20, 20));
    bg.setOutlineColor(sf::Color(180, 180, 180, 150));
    bg.setOutlineThickness(1.0f);
    window.draw(bg);
    
    // Health
    float healthPercent = static_cast<float>(current) / max;
    sf::RectangleShape health(sf::Vector2f(barWidth * healthPercent, barHeight));
    health.setPosition(sf::Vector2f(pos.x - barWidth/2, pos.y));
    health.setFillColor(sf::Color(80, 232, 102));
    window.draw(health);

    if (fontLoaded_) {
        sf::Text hpText(font_, std::to_string(current) + "/" + std::to_string(max), 12);
        hpText.setFillColor(sf::Color::White);
        const sf::FloatRect hpBounds = hpText.getLocalBounds();
        hpText.setPosition({pos.x - hpBounds.size.x * 0.5f - hpBounds.position.x, pos.y - 16.0f});
        window.draw(hpText);
    }
}

void HexRenderer::drawUI(sf::RenderTarget& window,
                         const GameState& state,
                         int myPlayerId,
                         const std::vector<std::string>& infoLines) {
    sf::RectangleShape panel(sf::Vector2f(370.0f, 150.0f));
    panel.setPosition(sf::Vector2f(12.0f, 12.0f));
    panel.setFillColor(sf::Color(28, 30, 38, 220));
    panel.setOutlineColor(sf::Color(128, 138, 164));
    panel.setOutlineThickness(2.0f);
    window.draw(panel);

    sf::RectangleShape textbox(sf::Vector2f(500.0f, 172.0f));
    textbox.setPosition(sf::Vector2f(12.0f, 584.0f));
    textbox.setFillColor(sf::Color(18, 20, 26, 220));
    textbox.setOutlineColor(sf::Color(110, 122, 150));
    textbox.setOutlineThickness(2.0f);
    window.draw(textbox);

    if (!fontLoaded_) {
        return;
    }

    std::string phaseLabel = "Waiting";
    switch (state.phase) {
        case GamePhase::WAITING: phaseLabel = "Waiting"; break;
        case GamePhase::SELECTION: phaseLabel = "Selection"; break;
        case GamePhase::PLACEMENT: phaseLabel = "Placement"; break;
        case GamePhase::PLAYING: phaseLabel = "Battle"; break;
        case GamePhase::TURN_RESOLVE: phaseLabel = "Resolving"; break;
        case GamePhase::GAME_OVER: phaseLabel = "Game Over"; break;
    }

    sf::Text header(font_, "Player " + std::to_string(myPlayerId) + " | " + phaseLabel + " | Turn " + std::to_string(state.currentTurn), 20);
    header.setPosition({24.0f, 24.0f});
    header.setFillColor(sf::Color::White);
    window.draw(header);

    sf::Text infoTitle(font_, "Info", 22);
    infoTitle.setPosition({26.0f, 590.0f});
    infoTitle.setFillColor(sf::Color(212, 222, 250));
    window.draw(infoTitle);

    float y = 622.0f;
    for (auto it = infoLines.rbegin(); it != infoLines.rend(); ++it) {
        sf::Text msg(font_, *it, 18);
        msg.setPosition({26.0f, y});
        msg.setFillColor(sf::Color(230, 235, 246));
        window.draw(msg);
        y += 22.0f;
        if (y > 744.0f) {
            break;
        }
    }
}

std::optional<CharacterClass> HexRenderer::getSelectionClassAt(sf::Vector2f point) const {
    constexpr float startX = 100.0f;
    constexpr float startY = 150.0f;
    constexpr float cardWidth = 240.0f;
    constexpr float cardHeight = 220.0f;
    constexpr float gapX = 24.0f;
    constexpr float gapY = 24.0f;

    for (int i = 0; i < 6; ++i) {
        const float x = startX + (i % 3) * (cardWidth + gapX);
        const float y = startY + (i / 3) * (cardHeight + gapY);
        if (sf::FloatRect({x, y}, {cardWidth, cardHeight}).contains(point)) {
            return static_cast<CharacterClass>(i);
        }
    }
    return std::nullopt;
}

bool HexRenderer::isSelectionReadyButtonAt(sf::Vector2f point) const {
    const sf::FloatRect readyButton({370.0f, 630.0f}, {280.0f, 64.0f});
    return readyButton.contains(point);
}

void HexRenderer::drawSelectionScreen(sf::RenderTarget& window,
                                      const std::vector<CharacterClass>& selected,
                                      std::optional<CharacterClass> hoveredClass,
                                      bool readyEnabled,
                                      bool readySent,
                                      bool readyHovered,
                                      float elapsedSeconds,
                                      std::optional<CharacterClass> justSelectedClass,
                                      bool opponentReady) {
    constexpr float startX = 100.0f;
    constexpr float startY = 150.0f;
    constexpr float cardWidth = 240.0f;
    constexpr float cardHeight = 220.0f;
    constexpr float gapX = 24.0f;
    constexpr float gapY = 24.0f;

    if (fontLoaded_) {
        sf::Text title(font_, "Select 3 Classes", 40);
        title.setPosition({340.0f, 30.0f});
        title.setFillColor(sf::Color(235, 238, 245));
        window.draw(title);
    }

    for (int i = 0; i < 6; ++i) {
        const CharacterClass charClass = static_cast<CharacterClass>(i);
        const bool isHovered = hoveredClass.has_value() && hoveredClass.value() == charClass;
        const bool isSelected = std::find(selected.begin(), selected.end(), charClass) != selected.end();
        const bool isJustSelected = justSelectedClass.has_value() && justSelectedClass.value() == charClass;

        const float baseX = startX + (i % 3) * (cardWidth + gapX);
        const float baseY = startY + (i / 3) * (cardHeight + gapY);
        const float hoverLift = isHovered ? (6.0f + std::sin(elapsedSeconds * 8.0f) * 4.0f) : 0.0f;
        const float pulse = isJustSelected ? (std::sin(elapsedSeconds * 16.0f) * 0.5f + 0.5f) : 0.0f;

        sf::RectangleShape card({cardWidth, cardHeight});
        card.setPosition({baseX, baseY - hoverLift});
        card.setFillColor(sf::Color(30, 34, 44));
        card.setOutlineColor(isSelected ? sf::Color(102, 227, 142) : sf::Color(130, 138, 162));
        card.setOutlineThickness(isSelected ? (4.0f + pulse * 2.0f) : 2.0f);
        window.draw(card);

        // Class-colored top ribbon
        sf::RectangleShape accent({cardWidth, 40.0f});
        accent.setPosition({baseX, baseY - hoverLift});
        sf::Color classColor = getClassColor(charClass);
        accent.setFillColor(sf::Color(classColor.r, classColor.g, classColor.b, 220));
        window.draw(accent);

        const sf::Vector2f center(baseX + cardWidth * 0.5f, baseY + 118.0f - hoverLift);
        drawClassGlyph(window, charClass, center, 2.0f, classColor);

        if (fontLoaded_) {
            sf::Text className(font_, getClassName(charClass), 24);
            className.setFillColor(sf::Color::White);
            const sf::FloatRect nameBounds = className.getLocalBounds();
            className.setPosition({baseX + (cardWidth - nameBounds.size.x) * 0.5f - nameBounds.position.x,
                                   baseY + 8.0f - hoverLift});
            window.draw(className);
        }
    }

    // Selection summary
    if (fontLoaded_) {
        sf::Text summary(font_,
                         "Selected: " + std::to_string(selected.size()) + "/3",
                         28);
        summary.setPosition({100.0f, 608.0f});
        summary.setFillColor(sf::Color(235, 238, 245));
        window.draw(summary);

        const std::string readyStatus = opponentReady ? "Opponent: READY" : "Opponent: selecting...";
        sf::Text opp(font_, readyStatus, 22);
        opp.setPosition({100.0f, 648.0f});
        opp.setFillColor(opponentReady ? sf::Color(128, 236, 152) : sf::Color(224, 196, 106));
        window.draw(opp);
    }

    // Ready button
    sf::RectangleShape readyButton({280.0f, 64.0f});
    readyButton.setPosition({370.0f, 630.0f});
    if (!readyEnabled || readySent) {
        readyButton.setFillColor(sf::Color(88, 92, 102));
    } else {
        readyButton.setFillColor(readyHovered ? sf::Color(86, 189, 119) : sf::Color(66, 163, 98));
    }
    readyButton.setOutlineColor(sf::Color(180, 187, 205));
    readyButton.setOutlineThickness(2.0f);
    window.draw(readyButton);

    if (fontLoaded_) {
        const std::string buttonLabel = readySent ? "Waiting..." : "Ready";
        sf::Text readyText(font_, buttonLabel, 30);
        readyText.setFillColor(sf::Color::White);
        const sf::FloatRect textBounds = readyText.getLocalBounds();
        readyText.setPosition({370.0f + (280.0f - textBounds.size.x) * 0.5f - textBounds.position.x,
                               630.0f + (64.0f - textBounds.size.y) * 0.5f - textBounds.position.y});
        window.draw(readyText);
    }
}

void HexRenderer::drawPlacementScreen(sf::RenderTarget& window, 
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

void HexRenderer::drawArrow(sf::RenderTarget& window, sf::Vector2f from, sf::Vector2f to, sf::Color color) {
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

void HexRenderer::drawPendingActions(sf::RenderTarget& window, const GameState& state, 
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

void HexRenderer::drawSelectionHighlight(sf::RenderTarget& window, const HexCoord& hex) {
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

void HexRenderer::drawRangeOverlay(sf::RenderTarget& window,
                                   const HexCoord& center,
                                   int moveRange,
                                   int attackRange,
                                   bool showMove,
                                   bool showAttack) {
    if (!showMove && !showAttack) {
        return;
    }

    const int maxRange = std::max(moveRange, attackRange);
    const auto hexes = center.hexesInRange(maxRange);

    for (const HexCoord& hex : hexes) {
        if (!hex.isValid(GameState::GRID_RADIUS)) {
            continue;
        }

        const int dist = center.distance(hex);
        if (dist == 0) {
            continue;
        }

        const sf::Vector2f pos = hexToPixel(hex);
        sf::CircleShape marker(hexSize_ * 0.74f, 6);
        marker.setOrigin({hexSize_ * 0.74f, hexSize_ * 0.74f});
        marker.setPosition(pos);
        marker.setRotation(sf::degrees(30.0f));

        if (showMove && dist <= moveRange) {
            marker.setFillColor(sf::Color(120, 200, 255, 74));
            marker.setOutlineColor(sf::Color(148, 220, 255, 130));
            marker.setOutlineThickness(1.6f);
            window.draw(marker);
        }

        if (showAttack && dist <= attackRange) {
            marker.setFillColor(sf::Color::Transparent);
            marker.setOutlineColor(sf::Color(255, 138, 138, 145));
            marker.setOutlineThickness((showMove && dist <= moveRange) ? 1.9f : 2.3f);
            window.draw(marker);
        }
    }
}
