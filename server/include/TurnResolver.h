#pragma once

#include "GameState.h"
#include "Action.h"
#include <string>
#include <vector>

/**
 * Result of a single action execution
 */
struct ActionResult {
    std::string characterId;
    std::string description;
    bool success;
    int damageDealt;
    int healingDone;
    
    ActionResult() : success(false), damageDealt(0), healingDone(0) {}
};

/**
 * Resolves simultaneous player actions
 */
class TurnResolver {
public:
    TurnResolver(GameState& gameState);
    
    /**
     * Resolve both players' actions simultaneously
     * Returns a summary of what happened
     */
    std::string resolveTurn();
    
private:
    GameState& state;
    std::vector<ActionResult> results;
    
    // Action execution
    ActionResult executeAction(const Action& action, int playerId);
    ActionResult executeMoveAction(const Action& action, Character& character);
    ActionResult executeSkillAction(const Action& action, Character& character);
    
    // Skill resolution
    void applyDamage(Character& target, int damage, const std::string& source);
    void applyHealing(Character& target, int healing);
    void applyPush(const HexCoord& from, const HexCoord& targetPos, int distance);
    void applyTerrainChange(const HexCoord& pos, TerrainType newTerrain);
    
    // Combo detection
    bool detectCombo(const std::vector<Action>& allActions);
    
    // Conflict resolution (when two units try to move to same hex)
    void resolveMovementConflicts();
    
    // Summary generation
    std::string generateSummary() const;
};
