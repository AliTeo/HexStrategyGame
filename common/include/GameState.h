#pragma once

#include "HexCoord.h"
#include "Character.h"
#include "Action.h"
#include <unordered_map>
#include <vector>
#include <memory>
#include <nlohmann/json.hpp>

/**
 * Game phase enum
 */
enum class GamePhase {
    WAITING,        // Waiting for players
    SELECTION,      // Character selection
    PLACEMENT,      // Placing characters on grid
    PLAYING,        // Main game phase
    TURN_RESOLVE,   // Resolving simultaneous actions
    GAME_OVER       // Game ended
};

/**
 * Hex terrain types
 */
enum class TerrainType {
    NORMAL,
    BLOCKED,        // Impassable
    FIRE,           // Damage over time
    ICE,            // Slippery/altered movement
    VOID            // Out of bounds
};

/**
 * Represents the complete game state
 */
class GameState {
public:
    static const int GRID_RADIUS = 3;  // 7x7 grid (radius 3)
    static const int CHARS_PER_PLAYER = 3;
    static const int TOTAL_CHAR_POOL = 6;
    
    GamePhase phase;
    int currentTurn;
    
    // Players
    std::vector<int> playerIds;  // 0 and 1
    std::unordered_map<int, bool> playerReady;  // Ready status
    std::unordered_map<int, std::vector<CharacterClass>> selectedClasses;  // Character selections
    
    // Characters on the board
    std::unordered_map<std::string, std::shared_ptr<Character>> characters;
    std::unordered_map<HexCoord, std::string> occupancy;  // hex -> characterId
    
    // Terrain
    std::unordered_map<HexCoord, TerrainType> terrain;
    
    // Turn actions
    TurnActions player0Actions;
    TurnActions player1Actions;
    
    // Game result
    int winnerId;

    GameState();
    
    // Phase management
    void startSelection();
    void startPlacement();
    void startPlaying();
    void endGame(int winner);
    
    // Player management
    void addPlayer(int playerId);
    bool bothPlayersReady() const;
    void setPlayerReady(int playerId, bool ready);
    
    // Character selection
    bool selectCharacter(int playerId, CharacterClass charClass);
    bool hasCompletedSelection(int playerId) const;
    bool bothPlayersSelectedCharacters() const;
    
    // Character placement
    bool placeCharacter(int playerId, CharacterClass charClass, const HexCoord& pos);
    bool hasCompletedPlacement(int playerId) const;
    bool bothPlayersPlacedCharacters() const;
    bool isValidPlacement(int playerId, const HexCoord& pos) const;
    
    // Character management
    std::shared_ptr<Character> getCharacter(const std::string& charId) const;
    std::shared_ptr<Character> getCharacterAt(const HexCoord& pos) const;
    std::vector<std::shared_ptr<Character>> getPlayerCharacters(int playerId) const;
    bool moveCharacter(const std::string& charId, const HexCoord& newPos);
    
    // Terrain
    TerrainType getTerrainAt(const HexCoord& pos) const;
    void setTerrainAt(const HexCoord& pos, TerrainType type);
    bool isWalkable(const HexCoord& pos) const;
    bool isInBounds(const HexCoord& pos) const;
    
    // Turn management
    TurnActions& getPlayerActions(int playerId);
    bool bothPlayersLockedIn() const;
    void resetTurn();
    void incrementTurn();
    
    // Victory conditions
    bool checkVictory();
    bool hasLost(int playerId) const;
    
    // Serialization
    nlohmann::json toJson() const;
    static GameState fromJson(const nlohmann::json& j);
    
    // Debug
    std::string toString() const;
};
