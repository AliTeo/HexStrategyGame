#include "GameSession.h"

GameSession::GameSession(int sessionId) 
    : sessionId(sessionId), gameState(), turnResolver(std::make_unique<TurnResolver>(gameState)) {}

bool GameSession::addPlayer(int playerId) {
    if (isFull()) return false;
    gameState.addPlayer(playerId);
    return true;
}

bool GameSession::hasPlayer(int playerId) const {
    return std::find(gameState.playerIds.begin(), gameState.playerIds.end(), playerId) 
           != gameState.playerIds.end();
}

bool GameSession::isFull() const {
    return gameState.playerIds.size() >= 2;
}

int GameSession::getPlayerCount() const {
    return gameState.playerIds.size();
}

void GameSession::startSelection() {
    gameState.startSelection();
}

void GameSession::startPlacement() {
    gameState.startPlacement();
}

void GameSession::startGame() {
    gameState.startPlaying();
}

bool GameSession::handleSelectCharacter(int playerId, CharacterClass charClass) {
    if (gameState.phase != GamePhase::SELECTION) return false;
    if (gameState.playerReady.count(playerId) && gameState.playerReady[playerId]) return false;
    
    bool success = gameState.selectCharacter(playerId, charClass);
    return success;
}

bool GameSession::handleReady(int playerId) {
    if (gameState.phase != GamePhase::SELECTION) return false;
    if (!gameState.hasCompletedSelection(playerId)) return false;

    gameState.setPlayerReady(playerId, true);
    if (gameState.bothPlayersReady()) {
        startPlacement();
    }
    return true;
}

bool GameSession::handlePlaceCharacter(int playerId, CharacterClass charClass, const HexCoord& pos) {
    if (gameState.phase != GamePhase::PLACEMENT) return false;
    
    bool success = gameState.placeCharacter(playerId, charClass, pos);
    
    // Auto-advance to playing when both players have placed all characters
    if (success && gameState.bothPlayersPlacedCharacters()) {
        startGame();
    }
    
    return success;
}

bool GameSession::handleAction(const Action& action) {
    if (gameState.phase != GamePhase::PLAYING) return false;
    
    auto character = gameState.getCharacter(action.characterId);
    if (!character) return false;
    
    int playerId = character->playerId;
    gameState.getPlayerActions(playerId).addAction(action);
    
    return true;
}

bool GameSession::handleLockTurn(int playerId) {
    if (gameState.phase != GamePhase::PLAYING) return false;
    
    gameState.getPlayerActions(playerId).lock();
    return true;
}

bool GameSession::shouldResolveTurn() const {
    return gameState.phase == GamePhase::PLAYING && gameState.bothPlayersLockedIn();
}

std::string GameSession::resolveTurn() {
    if (!shouldResolveTurn()) {
        return "Cannot resolve turn - not ready";
    }
    
    gameState.phase = GamePhase::TURN_RESOLVE;
    std::string summary = turnResolver->resolveTurn();
    
    if (gameState.phase != GamePhase::GAME_OVER) {
        gameState.incrementTurn();
        gameState.phase = GamePhase::PLAYING;
    }
    
    return summary;
}
