#include "TextRenderer.h"
#include <sstream>
#include <iomanip>

TextRenderer::TextRenderer() {}

void TextRenderer::displayWelcome() {
    std::cout << "\n";
    std::cout << "Welcome to Hexagonal Strategy Game!\n";
    std::cout << "====================================\n\n";
}

void TextRenderer::displayLogo() {
    std::cout << "\n";
    std::cout << "  _   _              ____  _        _                     \n";
    std::cout << " | | | | _____  __  / ___|| |_ _ __| |_                  \n";
    std::cout << " | |_| |/ _ \\ \\/ /  \\___ \\|  _| '__| __|               \n";
    std::cout << " |  _  |  __/>  <    ___) | |_| |  | |_                  \n";
    std::cout << " |_| |_|\\___/_/\\_\\  |____/ \\__|_|   \\__|              \n";
    std::cout << "\n";
    std::cout << "         [PROTOTYPE - TEXT MODE]\n";
    std::cout << "\n";
}

void TextRenderer::displayGameState(const GameState& state, int myPlayerId) {
    std::cout << "\n========================================\n";
    std::cout << "GAME STATE\n";
    std::cout << "========================================\n";
    
    std::string phaseStr;
    switch (state.phase) {
        case GamePhase::WAITING: phaseStr = "Waiting for players"; break;
        case GamePhase::SELECTION: phaseStr = "Character Selection"; break;
        case GamePhase::PLACEMENT: phaseStr = "Character Placement"; break;
        case GamePhase::PLAYING: phaseStr = "Playing"; break;
        case GamePhase::TURN_RESOLVE: phaseStr = "Resolving Turn"; break;
        case GamePhase::GAME_OVER: phaseStr = "Game Over"; break;
    }
    
    std::cout << "Phase: " << phaseStr << "\n";
    std::cout << "Turn: " << state.currentTurn << "\n";
    std::cout << "Your Player ID: " << myPlayerId << "\n";
    std::cout << "========================================\n\n";
}

void TextRenderer::displayHexGrid(const GameState& state, int myPlayerId) {
    std::cout << "\nHEX GRID (7x7):\n";
    std::cout << "Legend: [P0] = Player 0, [P1] = Player 1, [  ] = Empty\n";
    std::cout << "        W=Warrior, M=Mage, H=Healer, R=Rogue, T=Tank, S=Summoner\n\n";
    
    int radius = GameState::GRID_RADIUS;
    
    for (int r = -radius; r <= radius; ++r) {
        // Indentation for visual hex layout
        int indent = std::abs(r);
        std::cout << std::string(indent * 2, ' ');
        
        for (int q = -radius; q <= radius; ++q) {
            HexCoord pos(q, r);
            
            if (!pos.isValid(radius)) {
                continue;
            }
            
            auto character = state.getCharacterAt(pos);
            if (character) {
                std::string symbol = getCharacterSymbol(*character);
                std::string playerMark = (character->playerId == 0) ? "P0" : "P1";
                std::cout << "[" << playerMark << symbol << "]";
            } else {
                TerrainType terrain = state.getTerrainAt(pos);
                std::string terrainSym = getTerrainSymbol(terrain);
                std::cout << "[" << terrainSym << "]";
            }
        }
        std::cout << "  (r=" << r << ")\n";
    }
    std::cout << "\n";
}

void TextRenderer::displayCharacterList(const GameState& state, int myPlayerId) {
    std::cout << "\nYOUR CHARACTERS:\n";
    std::cout << "----------------\n";
    
    auto characters = state.getPlayerCharacters(myPlayerId);
    if (characters.empty()) {
        std::cout << "No characters yet.\n";
        return;
    }
    
    for (const auto& character : characters) {
        std::cout << "  " << character->id << ": " << character->name 
                  << " (HP: " << character->stats.currentHp << "/" << character->stats.maxHp << ")";
        
        if (!character->isAlive) {
            std::cout << " [DEFEATED]";
        }
        
        // Find position
        for (const auto& [pos, charId] : state.occupancy) {
            if (charId == character->id) {
                std::cout << " at (" << pos.q << "," << pos.r << ")";
                break;
            }
        }
        
        std::cout << "\n";
        
        // Show skills
        std::cout << "    Skills: ";
        for (size_t i = 0; i < character->skills.size(); ++i) {
            if (i > 0) std::cout << ", ";
            std::cout << character->skills[i].id;
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}

void TextRenderer::displayAvailableClasses() {
    std::cout << "\nAVAILABLE CHARACTER CLASSES:\n";
    std::cout << "-----------------------------\n";
    std::cout << "0. Warrior  - Melee fighter (HP:120, ATK:15, DEF:8)\n";
    std::cout << "1. Mage     - Ranged damage (HP:80, ATK:20, DEF:3)\n";
    std::cout << "2. Healer   - Support healer (HP:90, ATK:8, DEF:5)\n";
    std::cout << "3. Rogue    - High mobility (HP:85, ATK:18, DEF:4)\n";
    std::cout << "4. Tank     - Heavy defense (HP:150, ATK:10, DEF:12)\n";
    std::cout << "5. Summoner - Hex control (HP:75, ATK:12, DEF:4)\n";
    std::cout << "\nSelect 3 characters for your team.\n\n";
}

void TextRenderer::displayTurnSummary(const std::string& summary) {
    std::cout << "\n" << summary << "\n";
}

void TextRenderer::displayError(const std::string& error) {
    std::cout << "\n⚠ ERROR: " << error << "\n\n";
}

void TextRenderer::displayMessage(const std::string& message) {
    std::cout << message << "\n";
}

void TextRenderer::displayHelp() {
    std::cout << "\nCOMMANDS:\n";
    std::cout << "---------\n";
    std::cout << "During Selection:\n";
    std::cout << "  select <0-5>          - Select a character class\n";
    std::cout << "\n";
    std::cout << "During Placement:\n";
    std::cout << "  place <class_id> <q> <r> - Place character at hex (q,r)\n";
    std::cout << "                             Player 0 uses q < 0, Player 1 uses q > 0\n";
    std::cout << "\n";
    std::cout << "During Playing:\n";
    std::cout << "  move <char_id> <q> <r>        - Move character to hex\n";
    std::cout << "  skill <char_id> <skill_id> <q> <r> - Use skill on target hex\n";
    std::cout << "  pass <char_id>                - Character does nothing\n";
    std::cout << "  ready                         - Lock in your turn\n";
    std::cout << "\n";
    std::cout << "General:\n";
    std::cout << "  view          - Display current game state\n";
    std::cout << "  help          - Show this help\n";
    std::cout << "  quit          - Disconnect from game\n";
    std::cout << "\n";
}

void TextRenderer::displayPrompt(const std::string& prompt) {
    std::cout << prompt << ": ";
}

void TextRenderer::clearScreen() {
    // Simple clear for cross-platform compatibility
    std::cout << "\n\n\n";
}

void TextRenderer::waitForInput() {
    std::cout << "\nPress Enter to continue...";
    std::cin.ignore();
    std::cin.get();
}

std::string TextRenderer::getCharacterSymbol(const Character& character) const {
    switch (character.charClass) {
        case CharacterClass::WARRIOR: return "W";
        case CharacterClass::MAGE: return "M";
        case CharacterClass::HEALER: return "H";
        case CharacterClass::ROGUE: return "R";
        case CharacterClass::TANK: return "T";
        case CharacterClass::SUMMONER: return "S";
        default: return "?";
    }
}

std::string TextRenderer::getTerrainSymbol(TerrainType terrain) const {
    switch (terrain) {
        case TerrainType::NORMAL: return "  ";
        case TerrainType::BLOCKED: return "##";
        case TerrainType::FIRE: return "FF";
        case TerrainType::ICE: return "II";
        case TerrainType::OUT_OF_BOUNDS: return "XX";
        default: return "??";
    }
}
