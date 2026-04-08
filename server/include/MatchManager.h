#pragma once

#include "GameSession.h"
#include <memory>
#include <map>

/**
 * Manages game sessions and matchmaking
 */
class MatchManager {
public:
    MatchManager();
    
    // Session management
    std::shared_ptr<GameSession> createSession();
    std::shared_ptr<GameSession> getSession(int sessionId);
    std::shared_ptr<GameSession> findAvailableSession();
    
    // Player assignment
    std::shared_ptr<GameSession> assignPlayerToSession(int playerId);
    std::shared_ptr<GameSession> getPlayerSession(int playerId);
    
    // Cleanup
    void removeSession(int sessionId);
    void removePlayerFromSessions(int playerId);
    
private:
    int nextSessionId;
    std::map<int, std::shared_ptr<GameSession>> sessions;
    std::map<int, int> playerToSession;  // playerId -> sessionId
};
