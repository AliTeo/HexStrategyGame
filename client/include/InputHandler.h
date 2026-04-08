#pragma once

#include <string>
#include <vector>
#include <functional>

/**
 * Handles user input parsing and command execution
 */
class InputHandler {
public:
    using CommandCallback = std::function<void(const std::vector<std::string>&)>;
    
    InputHandler();
    
    // Parse input string into command and arguments
    bool parseInput(const std::string& input, std::string& command, std::vector<std::string>& args);
    
    // Parse specific command types
    bool parseSelectCommand(const std::vector<std::string>& args, int& classId);
    bool parsePlaceCommand(const std::vector<std::string>& args, int& classId, int& q, int& r);
    bool parseMoveCommand(const std::vector<std::string>& args, std::string& charId, int& q, int& r);
    bool parseSkillCommand(const std::vector<std::string>& args, 
                          std::string& charId, std::string& skillId, int& q, int& r);
    bool parsePassCommand(const std::vector<std::string>& args, std::string& charId);
    
    // Utility
    std::string trim(const std::string& str);
    std::vector<std::string> split(const std::string& str, char delimiter = ' ');
};
