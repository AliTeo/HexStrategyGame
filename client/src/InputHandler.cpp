#include "InputHandler.h"
#include <sstream>
#include <algorithm>

InputHandler::InputHandler() {}

bool InputHandler::parseInput(const std::string& input, std::string& command, std::vector<std::string>& args) {
    std::string trimmedInput = trim(input);
    if (trimmedInput.empty()) {
        return false;
    }
    
    auto parts = split(trimmedInput);
    if (parts.empty()) {
        return false;
    }
    
    command = parts[0];
    std::transform(command.begin(), command.end(), command.begin(), ::tolower);
    
    args.clear();
    for (size_t i = 1; i < parts.size(); ++i) {
        args.push_back(parts[i]);
    }
    
    return true;
}

bool InputHandler::parseSelectCommand(const std::vector<std::string>& args, int& classId) {
    if (args.empty()) {
        return false;
    }
    
    try {
        classId = std::stoi(args[0]);
        return classId >= 0 && classId < 6;
    } catch (...) {
        return false;
    }
}

bool InputHandler::parsePlaceCommand(const std::vector<std::string>& args, int& classId, int& q, int& r) {
    if (args.size() < 3) {
        return false;
    }
    
    try {
        classId = std::stoi(args[0]);
        q = std::stoi(args[1]);
        r = std::stoi(args[2]);
        return true;
    } catch (...) {
        return false;
    }
}

bool InputHandler::parseMoveCommand(const std::vector<std::string>& args, std::string& charId, int& q, int& r) {
    if (args.size() < 3) {
        return false;
    }
    
    try {
        charId = args[0];
        q = std::stoi(args[1]);
        r = std::stoi(args[2]);
        return true;
    } catch (...) {
        return false;
    }
}

bool InputHandler::parseSkillCommand(const std::vector<std::string>& args, 
                                     std::string& charId, std::string& skillId, int& q, int& r) {
    if (args.size() < 4) {
        return false;
    }
    
    try {
        charId = args[0];
        skillId = args[1];
        q = std::stoi(args[2]);
        r = std::stoi(args[3]);
        return true;
    } catch (...) {
        return false;
    }
}

bool InputHandler::parsePassCommand(const std::vector<std::string>& args, std::string& charId) {
    if (args.empty()) {
        return false;
    }
    
    charId = args[0];
    return true;
}

std::string InputHandler::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) {
        return "";
    }
    
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, last - first + 1);
}

std::vector<std::string> InputHandler::split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    
    while (std::getline(ss, token, delimiter)) {
        token = trim(token);
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }
    
    return tokens;
}
