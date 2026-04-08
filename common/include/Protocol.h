#pragma once

#include "GameState.h"
#include <nlohmann/json.hpp>
#include <string>

/**
 * Message types for client-server communication
 */
enum class MessageType {
    // Client -> Server
    CONNECT,
    SELECT_CHARACTER,
    PLACE_CHARACTER,
    SUBMIT_ACTION,
    LOCK_TURN,
    READY,
    
    // Server -> Client
    CONNECTED,
    GAME_STATE_UPDATE,
    TURN_RESOLVED,
    ERROR_MSG,
    PLAYER_JOINED,
    GAME_STARTED,
    
    // Both
    DISCONNECT,
    PING,
    PONG
};

/**
 * Protocol handler for serializing/deserializing network messages
 */
class Protocol {
public:
    /**
     * Create a message from type and data
     */
    static std::string createMessage(MessageType type, const nlohmann::json& data);
    
    /**
     * Parse a message string into type and data
     */
    static bool parseMessage(const std::string& message, MessageType& type, nlohmann::json& data);
    
    // Convenience methods for specific message types
    
    // Client messages
    static std::string connectMessage(int playerId);
    static std::string selectCharacterMessage(int playerId, CharacterClass charClass);
    static std::string placeCharacterMessage(int playerId, CharacterClass charClass, const HexCoord& pos);
    static std::string submitActionMessage(const Action& action);
    static std::string lockTurnMessage(int playerId);
    static std::string readyMessage(int playerId);
    
    // Server messages
    static std::string connectedMessage(int assignedPlayerId);
    static std::string gameStateUpdateMessage(const GameState& state);
    static std::string turnResolvedMessage(const std::string& summary);
    static std::string errorMessage(const std::string& error);
    static std::string playerJoinedMessage(int playerId);
    static std::string gameStartedMessage();
    
    // Utility
    static std::string addDelimiter(const std::string& message);
    static std::vector<std::string> extractMessages(std::string& buffer);
    
    static const std::string MESSAGE_DELIMITER;
};
