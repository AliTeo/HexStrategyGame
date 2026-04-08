#pragma once

#include "GameState.h"
#include "TurnResolver.h"
#include <memory>
#include <string>

/**
 * Manages a single game session between two players
 */
class GameSession {
public:
    GameSession(int sessionId);
    
    int getSessionId() const { return sessionId; }
    GameState& getGameState() { return gameState; }
    const GameState& getGameState() const { return gameState; }
    
    // Player management
    bool addPlayer(int playerId);
    bool hasPlayer(int playerId) const;
    bool isFull() const;
    int getPlayerCount() const;
    
    // Game flow
    void startSelection();
    void startPlacement();
    void startGame();
    
    // Actions
    bool handleSelectCharacter(int playerId, CharacterClass charClass);
    bool handlePlaceCharacter(int playerId, CharacterClass charClass, const HexCoord& pos);
    bool handleAction(const Action& action);
    bool handleLockTurn(int playerId);
    
    // Turn resolution
    std::string resolveTurn();
    bool shouldResolveTurn() const;
    
private:
    int sessionId;
    GameState gameState;
    std::unique_ptr<TurnResolver> turnResolver;
};
