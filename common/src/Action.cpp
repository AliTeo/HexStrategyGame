#include "Action.h"
#include <algorithm>

// Action implementation
Action::Action() 
    : type(ActionType::PASS), target(0, 0) {}

Action::Action(ActionType type, const std::string& characterId)
    : type(type), characterId(characterId), target(0, 0) {}

Action::Action(ActionType type, const std::string& characterId, const HexCoord& target)
    : type(type), characterId(characterId), target(target) {}

Action::Action(const std::string& characterId, const std::string& skillId, const HexCoord& target)
    : type(ActionType::USE_SKILL), characterId(characterId), target(target), skillId(skillId) {}

bool Action::isValid() const {
    if (characterId.empty()) return false;
    if (type == ActionType::USE_SKILL && skillId.empty()) return false;
    return true;
}

nlohmann::json Action::toJson() const {
    nlohmann::json j{
        {"type", static_cast<int>(type)},
        {"characterId", characterId},
        {"target", target.toJson()}
    };
    
    if (!skillId.empty()) {
        j["skillId"] = skillId;
    }
    
    return j;
}

Action Action::fromJson(const nlohmann::json& j) {
    Action action;
    action.type = static_cast<ActionType>(j["type"].get<int>());
    action.characterId = j["characterId"];
    action.target = HexCoord::fromJson(j["target"]);
    
    if (j.contains("skillId")) {
        action.skillId = j["skillId"];
    }
    
    return action;
}

// TurnActions implementation
TurnActions::TurnActions() 
    : playerId(0), isLocked(false) {}

TurnActions::TurnActions(int playerId)
    : playerId(playerId), isLocked(false) {}

void TurnActions::addAction(const Action& action) {
    if (!isLocked) {
        // Remove existing action for same character
        actions.erase(
            std::remove_if(actions.begin(), actions.end(),
                [&action](const Action& a) { return a.characterId == action.characterId; }),
            actions.end()
        );
        actions.push_back(action);
    }
}

void TurnActions::clear() {
    if (!isLocked) {
        actions.clear();
    }
}

void TurnActions::lock() {
    isLocked = true;
}

bool TurnActions::hasActionFor(const std::string& characterId) const {
    return std::any_of(actions.begin(), actions.end(),
        [&characterId](const Action& a) { return a.characterId == characterId; });
}

nlohmann::json TurnActions::toJson() const {
    nlohmann::json actionsJson = nlohmann::json::array();
    for (const auto& action : actions) {
        actionsJson.push_back(action.toJson());
    }
    
    return nlohmann::json{
        {"playerId", playerId},
        {"actions", actionsJson},
        {"isLocked", isLocked}
    };
}

TurnActions TurnActions::fromJson(const nlohmann::json& j) {
    TurnActions turnActions(j["playerId"]);
    turnActions.isLocked = j["isLocked"];
    
    for (const auto& actionJson : j["actions"]) {
        turnActions.actions.push_back(Action::fromJson(actionJson));
    }
    
    return turnActions;
}
