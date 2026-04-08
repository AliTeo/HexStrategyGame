#include "TurnResolver.h"
#include <sstream>
#include <algorithm>

TurnResolver::TurnResolver(GameState& gameState) : state(gameState) {}

std::string TurnResolver::resolveTurn() {
    results.clear();
    
    // Collect all actions
    std::vector<Action> allActions;
    allActions.insert(allActions.end(), 
        state.player0Actions.actions.begin(), 
        state.player0Actions.actions.end());
    allActions.insert(allActions.end(), 
        state.player1Actions.actions.begin(), 
        state.player1Actions.actions.end());
    
    // Execute all actions simultaneously
    for (const auto& action : allActions) {
        auto character = state.getCharacter(action.characterId);
        if (character && character->isAlive) {
            ActionResult result = executeAction(action, character->playerId);
            results.push_back(result);
        }
    }
    
    // Check victory condition
    state.checkVictory();
    
    return generateSummary();
}

ActionResult TurnResolver::executeAction(const Action& action, int playerId) {
    auto character = state.getCharacter(action.characterId);
    if (!character) {
        ActionResult result;
        result.success = false;
        result.description = "Character not found";
        return result;
    }
    
    switch (action.type) {
        case ActionType::MOVE:
            return executeMoveAction(action, *character);
        case ActionType::USE_SKILL:
            return executeSkillAction(action, *character);
        case ActionType::PASS:
        default:
            ActionResult result;
            result.characterId = character->id;
            result.success = true;
            result.description = character->name + " passes";
            return result;
    }
}

ActionResult TurnResolver::executeMoveAction(const Action& action, Character& character) {
    ActionResult result;
    result.characterId = character.id;
    
    // Check if target is in movement range and walkable
    HexCoord currentPos;
    bool found = false;
    for (const auto& [pos, charId] : state.occupancy) {
        if (charId == character.id) {
            currentPos = pos;
            found = true;
            break;
        }
    }
    
    if (!found) {
        result.success = false;
        result.description = character.name + " not on board";
        return result;
    }
    
    int distance = currentPos.distance(action.target);
    if (distance > character.stats.movement) {
        result.success = false;
        result.description = character.name + " too far to move";
        return result;
    }
    
    if (state.moveCharacter(character.id, action.target)) {
        result.success = true;
        result.description = character.name + " moves to (" + 
            std::to_string(action.target.q) + "," + 
            std::to_string(action.target.r) + ")";
    } else {
        result.success = false;
        result.description = character.name + " cannot move there";
    }
    
    return result;
}

ActionResult TurnResolver::executeSkillAction(const Action& action, Character& character) {
    ActionResult result;
    result.characterId = character.id;
    
    // Find the skill
    Skill* skill = nullptr;
    for (auto& s : character.skills) {
        if (s.id == action.skillId) {
            skill = &s;
            break;
        }
    }
    
    if (!skill) {
        result.success = false;
        result.description = "Skill not found";
        return result;
    }
    
    // Get character position
    HexCoord charPos;
    for (const auto& [pos, charId] : state.occupancy) {
        if (charId == character.id) {
            charPos = pos;
            break;
        }
    }
    
    // Check range
    if (charPos.distance(action.target) > skill->range) {
        result.success = false;
        result.description = character.name + "'s " + skill->name + " out of range";
        return result;
    }
    
    // Execute skill based on type
    result.success = true;
    std::ostringstream desc;
    desc << character.name << " uses " << skill->name;
    
    switch (skill->type) {
        case SkillType::DAMAGE: {
            auto target = state.getCharacterAt(action.target);
            if (target && target->playerId != character.playerId) {
                int damage = skill->power + character.stats.attack;
                applyDamage(*target, damage, character.name);
                result.damageDealt = damage;
                desc << " dealing " << damage << " damage to " << target->name;
            } else {
                desc << " but misses";
            }
            break;
        }
        case SkillType::HEAL: {
            auto target = state.getCharacterAt(action.target);
            if (target && target->playerId == character.playerId) {
                applyHealing(*target, skill->power);
                result.healingDone = skill->power;
                desc << " healing " << target->name << " for " << skill->power;
            }
            break;
        }
        case SkillType::TERRAIN_ALTER: {
            state.setTerrainAt(action.target, TerrainType::ICE);
            desc << " altering terrain at (" << action.target.q << "," << action.target.r << ")";
            break;
        }
        case SkillType::PUSH: {
            auto target = state.getCharacterAt(action.target);
            if (target) {
                desc << " pushing " << target->name;
                applyPush(charPos, action.target, 1);
            }
            break;
        }
        default:
            desc << " (effect not yet implemented)";
            break;
    }
    
    result.description = desc.str();
    return result;
}

void TurnResolver::applyDamage(Character& target, int damage, const std::string& source) {
    target.takeDamage(damage);
}

void TurnResolver::applyHealing(Character& target, int healing) {
    target.heal(healing);
}

void TurnResolver::applyPush(const HexCoord& from, const HexCoord& targetPos, int distance) {
    auto target = state.getCharacterAt(targetPos);
    if (!target) return;
    
    // Calculate push direction
    HexCoord direction = targetPos - from;
    HexCoord newPos = targetPos + direction;
    
    if (state.isWalkable(newPos)) {
        state.moveCharacter(target->id, newPos);
    }
}

void TurnResolver::applyTerrainChange(const HexCoord& pos, TerrainType newTerrain) {
    state.setTerrainAt(pos, newTerrain);
}

bool TurnResolver::detectCombo(const std::vector<Action>& allActions) {
    // Simplified combo detection for prototype
    return false;
}

void TurnResolver::resolveMovementConflicts() {
    // Handle conflicts when multiple units move to same hex
    // For prototype, last one wins (already handled by state.moveCharacter)
}

std::string TurnResolver::generateSummary() const {
    std::ostringstream summary;
    summary << "=== Turn " << state.currentTurn << " Results ===\n";
    
    for (const auto& result : results) {
        summary << "  " << result.description << "\n";
    }
    
    // Add casualties
    for (const auto& [id, character] : state.characters) {
        if (!character->isAlive) {
            summary << "  ⚠ " << character->name << " has been defeated!\n";
        }
    }
    
    summary << "======================\n";
    return summary.str();
}
