#pragma once

#include "HexCoord.h"
#include "Character.h"
#include <string>
#include <nlohmann/json.hpp>

/**
 * Action types players can perform
 */
enum class ActionType {
    MOVE,           // Move to a hex
    USE_SKILL,      // Use a skill
    PASS            // Do nothing
};

/**
 * Represents a single action for one character
 */
class Action {
public:
    ActionType type;
    std::string characterId;    // Which character performs this action
    HexCoord target;            // Target hex coordinate
    std::string skillId;        // For USE_SKILL actions
    
    Action();
    Action(ActionType type, const std::string& characterId);
    Action(ActionType type, const std::string& characterId, const HexCoord& target);
    Action(const std::string& characterId, const std::string& skillId, const HexCoord& target);

    bool isValid() const;
    
    nlohmann::json toJson() const;
    static Action fromJson(const nlohmann::json& j);
};

/**
 * Collection of actions for one player's turn
 */
class TurnActions {
public:
    int playerId;
    std::vector<Action> actions;
    bool isLocked;  // Player has committed their turn
    
    TurnActions();
    TurnActions(int playerId);
    
    void addAction(const Action& action);
    void clear();
    void lock();
    bool hasActionFor(const std::string& characterId) const;
    
    nlohmann::json toJson() const;
    static TurnActions fromJson(const nlohmann::json& j);
};
