#pragma once

#include <string>

/**
 * Menu system (placeholder for future)
 */
class MenuSystem {
public:
    MenuSystem();
    
    // Menu states
    void showMainMenu();
    void showPauseMenu();
    void showGameOverMenu(int winnerId);
    
    // Get user selection
    int getMenuChoice(int minChoice, int maxChoice);
    
private:
    void displayMenuHeader(const std::string& title);
};
