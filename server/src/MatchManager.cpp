#include "MatchManager.h"

MatchManager::MatchManager() : nextSessionId(1) {}

std::shared_ptr<GameSession> MatchManager::createSession() {
    auto session = std::make_shared<GameSession>(nextSessionId++);
    sessions[session->getSessionId()] = session;
    return session;
}

std::shared_ptr<GameSession> MatchManager::getSession(int sessionId) {
    auto it = sessions.find(sessionId);
    return (it != sessions.end()) ? it->second : nullptr;
}

std::shared_ptr<GameSession> MatchManager::findAvailableSession() {
    for (auto& [id, session] : sessions) {
        if (!session->isFull()) {
            return session;
        }
    }
    return nullptr;
}

std::shared_ptr<GameSession> MatchManager::assignPlayerToSession(int playerId) {
    // Check if player already in a session
    if (playerToSession.count(playerId)) {
        return getSession(playerToSession[playerId]);
    }
    
    // Find available session or create new one
    auto session = findAvailableSession();
    if (!session) {
        session = createSession();
    }
    
    if (session->addPlayer(playerId)) {
        playerToSession[playerId] = session->getSessionId();
        
        // Start character selection when both players join
        if (session->isFull()) {
            session->startSelection();
        }
        
        return session;
    }
    
    return nullptr;
}

std::shared_ptr<GameSession> MatchManager::getPlayerSession(int playerId) {
    auto it = playerToSession.find(playerId);
    if (it != playerToSession.end()) {
        return getSession(it->second);
    }
    return nullptr;
}

void MatchManager::removeSession(int sessionId) {
    sessions.erase(sessionId);
    
    // Remove player mappings
    for (auto it = playerToSession.begin(); it != playerToSession.end(); ) {
        if (it->second == sessionId) {
            it = playerToSession.erase(it);
        } else {
            ++it;
        }
    }
}

void MatchManager::removePlayerFromSessions(int playerId) {
    playerToSession.erase(playerId);
}
