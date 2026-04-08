#include "MenuSystem.h"
#include <iostream>

MenuSystem::MenuSystem() {}

void MenuSystem::displayMenuHeader(const std::string& title) {
    std::cout << "\n";
    std::cout << "========================================\n";
    std::cout << "  " << title << "\n";
    std::cout << "========================================\n";
}

void MenuSystem::showMainMenu() {
    displayMenuHeader("MAIN MENU");
    std::cout << "1. Connect to Server\n";
    std::cout << "2. Settings (Not implemented)\n";
    std::cout << "3. Credits (Not implemented)\n";
    std::cout << "4. Exit\n";
    std::cout << "\n";
}

void MenuSystem::showPauseMenu() {
    displayMenuHeader("PAUSED");
    std::cout << "1. Resume\n";
    std::cout << "2. Help\n";
    std::cout << "3. Disconnect\n";
    std::cout << "\n";
}

void MenuSystem::showGameOverMenu(int winnerId) {
    displayMenuHeader("GAME OVER");
    std::cout << "Player " << winnerId << " wins!\n";
    std::cout << "\n";
    std::cout << "1. Return to Main Menu\n";
    std::cout << "2. Exit\n";
    std::cout << "\n";
}

int MenuSystem::getMenuChoice(int minChoice, int maxChoice) {
    int choice;
    std::cout << "Choice: ";
    std::cin >> choice;
    std::cin.ignore(1000, '\n');
    
    if (choice < minChoice || choice > maxChoice) {
        std::cout << "Invalid choice!\n";
        return -1;
    }
    
    return choice;
}
