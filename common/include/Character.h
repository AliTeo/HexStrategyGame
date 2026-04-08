#pragma once

#include <string>
#include <vector>
#include <memory>
#include <nlohmann/json.hpp>

// Forward declarations
class Character;
class Skill;

/**
 * Skill effects and types
 */
enum class SkillType {
    DAMAGE,           // Direct damage
    HEAL,             // Restore HP
    MOVE,             // Move target
    PUSH,             // Push target away
    PULL,             // Pull target closer
    TERRAIN_ALTER,    // Change hex terrain
    BUFF,             // Increase stats
    DEBUFF,           // Decrease stats
    SUMMON,           // Create temporary unit
    SHIELD            // Absorb damage
};

enum class TargetType {
    SELF,             // Caster only
    ALLY,             // Allied character
    ENEMY,            // Enemy character
    HEX,              // Hex tile (empty or occupied)
    AREA              // Area of hexes
};

/**
 * Skill class representing character abilities
 */
class Skill {
public:
    std::string id;
    std::string name;
    std::string description;
    
    SkillType type;
    TargetType targetType;
    
    int range;        // Max range in hexes
    int power;        // Damage/heal/effect strength
    int areaSize;     // Area of effect radius
    int cooldown;     // Turns until can use again
    
    // Combo system
    std::vector<std::string> combosWith;  // Skill IDs that combo
    std::string comboEffect;              // Description of combo
    int comboPowerBonus;                  // Additional power when comboed

    Skill();
    Skill(const std::string& id, const std::string& name, 
          SkillType type, TargetType targetType,
          int range, int power, int areaSize = 0);

    nlohmann::json toJson() const;
    static Skill fromJson(const nlohmann::json& j);
};

/**
 * Character stats
 */
struct CharacterStats {
    int maxHp;
    int currentHp;
    int attack;
    int defense;
    int movement;
    
    CharacterStats();
    CharacterStats(int hp, int atk, int def, int mov);
    
    nlohmann::json toJson() const;
    static CharacterStats fromJson(const nlohmann::json& j);
};

/**
 * Character class representing playable units
 */
enum class CharacterClass {
    WARRIOR,
    MAGE,
    HEALER,
    ROGUE,
    TANK,
    SUMMONER
};

class Character {
public:
    std::string id;
    std::string name;
    CharacterClass charClass;
    
    CharacterStats stats;
    std::vector<Skill> skills;
    
    int playerId;     // 0 or 1
    bool isAlive;

    Character();
    Character(const std::string& id, const std::string& name, 
              CharacterClass charClass, const CharacterStats& stats);

    void addSkill(const Skill& skill);
    void takeDamage(int damage);
    void heal(int amount);
    bool canUseSkill(const Skill& skill) const;

    nlohmann::json toJson() const;
    static Character fromJson(const nlohmann::json& j);

    // Factory methods for predefined characters
    static Character createWarrior(const std::string& id, int playerId);
    static Character createMage(const std::string& id, int playerId);
    static Character createHealer(const std::string& id, int playerId);
    static Character createRogue(const std::string& id, int playerId);
    static Character createTank(const std::string& id, int playerId);
    static Character createSummoner(const std::string& id, int playerId);
    
    // Get all available character classes
    static std::vector<std::string> getAvailableClasses();
    static Character createByClass(CharacterClass cls, const std::string& id, int playerId);
};
