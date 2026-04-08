#include "GameState.h"
#include <sstream>
#include <algorithm>
#include <iostream>

GameState::GameState() 
    : phase(GamePhase::WAITING), currentTurn(0), 
      player0Actions(0), player1Actions(1), winnerId(-1) {
    
    // Initialize terrain
    for (int q = -GRID_RADIUS; q <= GRID_RADIUS; ++q) {
        for (int r = -GRID_RADIUS; r <= GRID_RADIUS; ++r) {
            HexCoord pos(q, r);
            if (pos.isValid(GRID_RADIUS)) {
                terrain[pos] = TerrainType::NORMAL;
            }
        }
    }
}

// Phase management
void GameState::startSelection() {
    phase = GamePhase::SELECTION;
}

void GameState::startPlacement() {
    phase = GamePhase::PLACEMENT;
    
    // Create characters based on selections
    for (int playerId = 0; playerId <= 1; ++playerId) {
        if (selectedClasses.count(playerId)) {
            int charIndex = 0;
            for (CharacterClass charClass : selectedClasses[playerId]) {
                std::string charId = "p" + std::to_string(playerId) + "_c" + std::to_string(charIndex);
                auto character = std::make_shared<Character>(
                    Character::createByClass(charClass, charId, playerId)
                );
                characters[charId] = character;
                charIndex++;
            }
        }
    }
}

void GameState::startPlaying() {
    phase = GamePhase::PLAYING;
    currentTurn = 1;
}

void GameState::endGame(int winner) {
    phase = GamePhase::GAME_OVER;
    winnerId = winner;
}

// Player management
void GameState::addPlayer(int playerId) {
    if (std::find(playerIds.begin(), playerIds.end(), playerId) == playerIds.end()) {
        playerIds.push_back(playerId);
        playerReady[playerId] = false;
    }
}

bool GameState::bothPlayersReady() const {
    return playerIds.size() == 2 && 
           playerReady.count(0) && playerReady.at(0) &&
           playerReady.count(1) && playerReady.at(1);
}

void GameState::setPlayerReady(int playerId, bool ready) {
    playerReady[playerId] = ready;
}

// Character selection
bool GameState::selectCharacter(int playerId, CharacterClass charClass) {
    if (selectedClasses[playerId].size() >= CHARS_PER_PLAYER) {
        return false;
    }
    selectedClasses[playerId].push_back(charClass);
    return true;
}

bool GameState::hasCompletedSelection(int playerId) const {
    return selectedClasses.count(playerId) && 
           selectedClasses.at(playerId).size() == CHARS_PER_PLAYER;
}

bool GameState::bothPlayersSelectedCharacters() const {
    return hasCompletedSelection(0) && hasCompletedSelection(1);
}

// Character placement
bool GameState::isValidPlacement(int playerId, const HexCoord& pos) const {
    // Player 0: left side (q <= -1)
    // Player 1: right side (q >= 1)
    if (!isInBounds(pos)) return false;
    if (occupancy.count(pos)) return false;  // Already occupied
    
    if (playerId == 0 && pos.q >= 0) return false;
    if (playerId == 1 && pos.q <= 0) return false;
    
    return true;
}

bool GameState::placeCharacter(int playerId, CharacterClass charClass, const HexCoord& pos) {
    if (!isValidPlacement(playerId, pos)) {
        return false;
    }
    
    // Position is already validated as empty in isValidPlacement
    // Find an unplaced character of this class for this player
    for (auto& [id, character] : characters) {
        if (character->playerId == playerId && 
            character->charClass == charClass) {
            
            // Check if this character is already placed
            bool alreadyPlaced = false;
            for (const auto& [hex, charId] : occupancy) {
                if (charId == id) {
                    alreadyPlaced = true;
                    break;
                }
            }
            
            if (!alreadyPlaced) {
                occupancy[pos] = id;
                return true;
            }
        }
    }
    
    return false;
}

bool GameState::hasCompletedPlacement(int playerId) const {
    int placedCount = 0;
    for (const auto& [pos, charId] : occupancy) {
        if (charId.find("p" + std::to_string(playerId)) == 0) {
            placedCount++;
        }
    }
    return placedCount == CHARS_PER_PLAYER;
}

bool GameState::bothPlayersPlacedCharacters() const {
    return hasCompletedPlacement(0) && hasCompletedPlacement(1);
}

// Character management
std::shared_ptr<Character> GameState::getCharacter(const std::string& charId) const {
    auto it = characters.find(charId);
    return (it != characters.end()) ? it->second : nullptr;
}

std::shared_ptr<Character> GameState::getCharacterAt(const HexCoord& pos) const {
    auto it = occupancy.find(pos);
    if (it != occupancy.end()) {
        return getCharacter(it->second);
    }
    return nullptr;
}

std::vector<std::shared_ptr<Character>> GameState::getPlayerCharacters(int playerId) const {
    std::vector<std::shared_ptr<Character>> result;
    for (const auto& [id, character] : characters) {
        if (character->playerId == playerId) {
            result.push_back(character);
        }
    }
    return result;
}

bool GameState::moveCharacter(const std::string& charId, const HexCoord& newPos) {
    if (!isWalkable(newPos) || !isInBounds(newPos)) {
        return false;
    }
    
    // Find current position
    HexCoord oldPos;
    bool found = false;
    for (const auto& [pos, id] : occupancy) {
        if (id == charId) {
            oldPos = pos;
            found = true;
            break;
        }
    }
    
    if (!found) return false;
    
    // Move character
    occupancy.erase(oldPos);
    occupancy[newPos] = charId;
    return true;
}

// Terrain
TerrainType GameState::getTerrainAt(const HexCoord& pos) const {
    auto it = terrain.find(pos);
    return (it != terrain.end()) ? it->second : TerrainType::OUT_OF_BOUNDS;
}

void GameState::setTerrainAt(const HexCoord& pos, TerrainType type) {
    if (isInBounds(pos)) {
        terrain[pos] = type;
    }
}

bool GameState::isWalkable(const HexCoord& pos) const {
    if (!isInBounds(pos)) return false;
    if (occupancy.count(pos)) return false;
    
    TerrainType t = getTerrainAt(pos);
    return t != TerrainType::BLOCKED && t != TerrainType::OUT_OF_BOUNDS;
}

bool GameState::isInBounds(const HexCoord& pos) const {
    return pos.isValid(GRID_RADIUS);
}

// Turn management
TurnActions& GameState::getPlayerActions(int playerId) {
    return (playerId == 0) ? player0Actions : player1Actions;
}

bool GameState::bothPlayersLockedIn() const {
    return player0Actions.isLocked && player1Actions.isLocked;
}

void GameState::resetTurn() {
    player0Actions = TurnActions(0);
    player1Actions = TurnActions(1);
}

void GameState::incrementTurn() {
    currentTurn++;
    resetTurn();
}

// Victory conditions
bool GameState::hasLost(int playerId) const {
    auto chars = getPlayerCharacters(playerId);
    return std::all_of(chars.begin(), chars.end(),
        [](const std::shared_ptr<Character>& c) { return !c->isAlive; });
}

bool GameState::checkVictory() {
    if (hasLost(0)) {
        endGame(1);
        return true;
    }
    if (hasLost(1)) {
        endGame(0);
        return true;
    }
    return false;
}

// Serialization
nlohmann::json GameState::toJson() const {
    nlohmann::json charactersJson = nlohmann::json::object();
    for (const auto& [id, character] : characters) {
        charactersJson[id] = character->toJson();
    }
    
    nlohmann::json occupancyJson = nlohmann::json::array();
    for (const auto& [pos, charId] : occupancy) {
        occupancyJson.push_back({
            {"pos", pos.toJson()},
            {"charId", charId}
        });
    }
    
    nlohmann::json selectedJson = nlohmann::json::object();
    for (const auto& [playerId, classes] : selectedClasses) {
        nlohmann::json classArray = nlohmann::json::array();
        for (auto cls : classes) {
            classArray.push_back(static_cast<int>(cls));
        }
        selectedJson[std::to_string(playerId)] = classArray;
    }
    
    return nlohmann::json{
        {"phase", static_cast<int>(phase)},
        {"currentTurn", currentTurn},
        {"characters", charactersJson},
        {"occupancy", occupancyJson},
        {"selectedClasses", selectedJson},
        {"player0Actions", player0Actions.toJson()},
        {"player1Actions", player1Actions.toJson()},
        {"winnerId", winnerId}
    };
}

GameState GameState::fromJson(const nlohmann::json& j) {
    GameState state;
    state.phase = static_cast<GamePhase>(j["phase"].get<int>());
    state.currentTurn = j["currentTurn"];
    state.winnerId = j["winnerId"];
    
    // Reconstruct characters
    for (auto& [id, charJson] : j["characters"].items()) {
        state.characters[id] = std::make_shared<Character>(Character::fromJson(charJson));
    }
    
    // Reconstruct occupancy
    for (const auto& entry : j["occupancy"]) {
        HexCoord pos = HexCoord::fromJson(entry["pos"]);
        std::string charId = entry["charId"];
        state.occupancy[pos] = charId;
    }
    
    // Reconstruct selected classes
    for (auto& [playerIdStr, classArray] : j["selectedClasses"].items()) {
        int playerId = std::stoi(playerIdStr);
        for (int cls : classArray) {
            state.selectedClasses[playerId].push_back(static_cast<CharacterClass>(cls));
        }
    }
    
    state.player0Actions = TurnActions::fromJson(j["player0Actions"]);
    state.player1Actions = TurnActions::fromJson(j["player1Actions"]);
    
    return state;
}

std::string GameState::toString() const {
    std::ostringstream oss;
    oss << "Turn: " << currentTurn << " | Phase: " << static_cast<int>(phase) << "\n";
    oss << "Player 0: " << getPlayerCharacters(0).size() << " chars | ";
    oss << "Player 1: " << getPlayerCharacters(1).size() << " chars\n";
    return oss.str();
}
