#include "Protocol.h"
#include <sstream>

const std::string Protocol::MESSAGE_DELIMITER = "\n###END###\n";

std::string Protocol::createMessage(MessageType type, const nlohmann::json& data) {
    nlohmann::json message{
        {"type", static_cast<int>(type)},
        {"data", data}
    };
    return message.dump();
}

bool Protocol::parseMessage(const std::string& message, MessageType& type, nlohmann::json& data) {
    try {
        nlohmann::json j = nlohmann::json::parse(message);
        type = static_cast<MessageType>(j["type"].get<int>());
        data = j["data"];
        return true;
    } catch (...) {
        return false;
    }
}

// Client messages
std::string Protocol::connectMessage(int playerId) {
    return createMessage(MessageType::CONNECT, {{"playerId", playerId}});
}

std::string Protocol::selectCharacterMessage(int playerId, CharacterClass charClass) {
    return createMessage(MessageType::SELECT_CHARACTER, {
        {"playerId", playerId},
        {"class", static_cast<int>(charClass)}
    });
}

std::string Protocol::placeCharacterMessage(int playerId, CharacterClass charClass, const HexCoord& pos) {
    return createMessage(MessageType::PLACE_CHARACTER, {
        {"playerId", playerId},
        {"class", static_cast<int>(charClass)},
        {"position", pos.toJson()}
    });
}

std::string Protocol::submitActionMessage(const Action& action) {
    return createMessage(MessageType::SUBMIT_ACTION, action.toJson());
}

std::string Protocol::lockTurnMessage(int playerId) {
    return createMessage(MessageType::LOCK_TURN, {{"playerId", playerId}});
}

std::string Protocol::readyMessage(int playerId) {
    return createMessage(MessageType::READY, {{"playerId", playerId}});
}

// Server messages
std::string Protocol::connectedMessage(int assignedPlayerId) {
    return createMessage(MessageType::CONNECTED, {{"playerId", assignedPlayerId}});
}

std::string Protocol::gameStateUpdateMessage(const GameState& state) {
    return createMessage(MessageType::GAME_STATE_UPDATE, state.toJson());
}

std::string Protocol::turnResolvedMessage(const std::string& summary) {
    return createMessage(MessageType::TURN_RESOLVED, {{"summary", summary}});
}

std::string Protocol::errorMessage(const std::string& error) {
    return createMessage(MessageType::ERROR_MSG, {{"error", error}});
}

std::string Protocol::playerJoinedMessage(int playerId) {
    return createMessage(MessageType::PLAYER_JOINED, {{"playerId", playerId}});
}

std::string Protocol::gameStartedMessage() {
    return createMessage(MessageType::GAME_STARTED, {});
}

// Utility
std::string Protocol::addDelimiter(const std::string& message) {
    return message + MESSAGE_DELIMITER;
}

std::vector<std::string> Protocol::extractMessages(std::string& buffer) {
    std::vector<std::string> messages;
    size_t pos;
    
    while ((pos = buffer.find(MESSAGE_DELIMITER)) != std::string::npos) {
        std::string message = buffer.substr(0, pos);
        if (!message.empty()) {
            messages.push_back(message);
        }
        buffer.erase(0, pos + MESSAGE_DELIMITER.length());
    }
    
    return messages;
}
