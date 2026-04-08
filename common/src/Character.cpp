#include "Character.h"
#include <algorithm>

// Skill implementation
Skill::Skill() 
    : type(SkillType::DAMAGE), targetType(TargetType::ENEMY),
      range(1), power(0), areaSize(0), cooldown(0), comboPowerBonus(0) {}

Skill::Skill(const std::string& id, const std::string& name,
             SkillType type, TargetType targetType,
             int range, int power, int areaSize)
    : id(id), name(name), type(type), targetType(targetType),
      range(range), power(power), areaSize(areaSize), cooldown(0), comboPowerBonus(0) {
    description = name;
}

nlohmann::json Skill::toJson() const {
    return nlohmann::json{
        {"id", id},
        {"name", name},
        {"description", description},
        {"type", static_cast<int>(type)},
        {"targetType", static_cast<int>(targetType)},
        {"range", range},
        {"power", power},
        {"areaSize", areaSize},
        {"cooldown", cooldown}
    };
}

Skill Skill::fromJson(const nlohmann::json& j) {
    Skill skill;
    skill.id = j["id"];
    skill.name = j["name"];
    skill.description = j.value("description", "");
    skill.type = static_cast<SkillType>(j["type"].get<int>());
    skill.targetType = static_cast<TargetType>(j["targetType"].get<int>());
    skill.range = j["range"];
    skill.power = j["power"];
    skill.areaSize = j["areaSize"];
    skill.cooldown = j["cooldown"];
    return skill;
}

// CharacterStats implementation
CharacterStats::CharacterStats() 
    : maxHp(100), currentHp(100), attack(10), defense(5), movement(3) {}

CharacterStats::CharacterStats(int hp, int atk, int def, int mov)
    : maxHp(hp), currentHp(hp), attack(atk), defense(def), movement(mov) {}

nlohmann::json CharacterStats::toJson() const {
    return nlohmann::json{
        {"maxHp", maxHp},
        {"currentHp", currentHp},
        {"attack", attack},
        {"defense", defense},
        {"movement", movement}
    };
}

CharacterStats CharacterStats::fromJson(const nlohmann::json& j) {
    CharacterStats stats;
    stats.maxHp = j["maxHp"];
    stats.currentHp = j["currentHp"];
    stats.attack = j["attack"];
    stats.defense = j["defense"];
    stats.movement = j["movement"];
    return stats;
}

// Character implementation
Character::Character()
    : charClass(CharacterClass::WARRIOR), playerId(0), isAlive(true) {}

Character::Character(const std::string& id, const std::string& name,
                     CharacterClass charClass, const CharacterStats& stats)
    : id(id), name(name), charClass(charClass), stats(stats), 
      playerId(0), isAlive(true) {}

void Character::addSkill(const Skill& skill) {
    skills.push_back(skill);
}

void Character::takeDamage(int damage) {
    int actualDamage = std::max(0, damage - stats.defense);
    stats.currentHp -= actualDamage;
    if (stats.currentHp <= 0) {
        stats.currentHp = 0;
        isAlive = false;
    }
}

void Character::heal(int amount) {
    stats.currentHp = std::min(stats.maxHp, stats.currentHp + amount);
}

bool Character::canUseSkill(const Skill& skill) const {
    return isAlive; // Simplified for now
}

nlohmann::json Character::toJson() const {
    nlohmann::json skillsJson = nlohmann::json::array();
    for (const auto& skill : skills) {
        skillsJson.push_back(skill.toJson());
    }
    
    return nlohmann::json{
        {"id", id},
        {"name", name},
        {"class", static_cast<int>(charClass)},
        {"stats", stats.toJson()},
        {"skills", skillsJson},
        {"playerId", playerId},
        {"isAlive", isAlive}
    };
}

Character Character::fromJson(const nlohmann::json& j) {
    Character character;
    character.id = j["id"];
    character.name = j["name"];
    character.charClass = static_cast<CharacterClass>(j["class"].get<int>());
    character.stats = CharacterStats::fromJson(j["stats"]);
    character.playerId = j["playerId"];
    character.isAlive = j["isAlive"];
    
    for (const auto& skillJson : j["skills"]) {
        character.skills.push_back(Skill::fromJson(skillJson));
    }
    
    return character;
}

// Factory methods for each class
Character Character::createWarrior(const std::string& id, int playerId) {
    CharacterStats stats(120, 15, 8, 3);
    Character warrior(id, "Warrior", CharacterClass::WARRIOR, stats);
    warrior.playerId = playerId;
    
    warrior.addSkill(Skill("slash", "Power Slash", SkillType::DAMAGE, TargetType::ENEMY, 1, 25));
    warrior.addSkill(Skill("charge", "Charge", SkillType::PUSH, TargetType::ENEMY, 2, 15, 1));
    
    return warrior;
}

Character Character::createMage(const std::string& id, int playerId) {
    CharacterStats stats(80, 20, 3, 2);
    Character mage(id, "Mage", CharacterClass::MAGE, stats);
    mage.playerId = playerId;
    
    mage.addSkill(Skill("fireball", "Fireball", SkillType::DAMAGE, TargetType::ENEMY, 4, 30));
    mage.addSkill(Skill("ice_wall", "Ice Wall", SkillType::TERRAIN_ALTER, TargetType::HEX, 3, 0));
    
    return mage;
}

Character Character::createHealer(const std::string& id, int playerId) {
    CharacterStats stats(90, 8, 5, 3);
    Character healer(id, "Healer", CharacterClass::HEALER, stats);
    healer.playerId = playerId;
    
    healer.addSkill(Skill("heal", "Heal", SkillType::HEAL, TargetType::ALLY, 3, 35));
    healer.addSkill(Skill("shield", "Shield", SkillType::SHIELD, TargetType::ALLY, 2, 20));
    
    return healer;
}

Character Character::createRogue(const std::string& id, int playerId) {
    CharacterStats stats(85, 18, 4, 5);
    Character rogue(id, "Rogue", CharacterClass::ROGUE, stats);
    rogue.playerId = playerId;
    
    rogue.addSkill(Skill("backstab", "Backstab", SkillType::DAMAGE, TargetType::ENEMY, 1, 40));
    rogue.addSkill(Skill("dash", "Dash", SkillType::MOVE, TargetType::SELF, 0, 3));
    
    return rogue;
}

Character Character::createTank(const std::string& id, int playerId) {
    CharacterStats stats(150, 10, 12, 2);
    Character tank(id, "Tank", CharacterClass::TANK, stats);
    tank.playerId = playerId;
    
    tank.addSkill(Skill("taunt", "Taunt", SkillType::PULL, TargetType::ENEMY, 3, 10, 2));
    tank.addSkill(Skill("fortify", "Fortify", SkillType::BUFF, TargetType::SELF, 0, 15));
    
    return tank;
}

Character Character::createSummoner(const std::string& id, int playerId) {
    CharacterStats stats(75, 12, 4, 2);
    Character summoner(id, "Summoner", CharacterClass::SUMMONER, stats);
    summoner.playerId = playerId;
    
    summoner.addSkill(Skill("summon", "Summon Minion", SkillType::SUMMON, TargetType::HEX, 2, 20));
    summoner.addSkill(Skill("control", "Hex Control", SkillType::TERRAIN_ALTER, TargetType::AREA, 3, 0, 1));
    
    return summoner;
}

std::vector<std::string> Character::getAvailableClasses() {
    return {"Warrior", "Mage", "Healer", "Rogue", "Tank", "Summoner"};
}

Character Character::createByClass(CharacterClass cls, const std::string& id, int playerId) {
    switch (cls) {
        case CharacterClass::WARRIOR: return createWarrior(id, playerId);
        case CharacterClass::MAGE: return createMage(id, playerId);
        case CharacterClass::HEALER: return createHealer(id, playerId);
        case CharacterClass::ROGUE: return createRogue(id, playerId);
        case CharacterClass::TANK: return createTank(id, playerId);
        case CharacterClass::SUMMONER: return createSummoner(id, playerId);
        default: return createWarrior(id, playerId);
    }
}
