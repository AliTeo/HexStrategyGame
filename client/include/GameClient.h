#pragma once

#include "GameState.h"
#include "Protocol.h"
#include "TextRenderer.h"
#include "InputHandler.h"
#include "MenuSystem.h"
#include <boost/asio.hpp>
#include <memory>
#include <string>
#include <thread>
#include <chrono>

using boost::asio::ip::tcp;

/**
 * Main game client
 */
class GameClient {
public:
    GameClient();
    ~GameClient();
    
    // Connection
    bool connect(const std::string& host, const std::string& port);
    void disconnect();
    bool isConnected() const { return connected_; }
    
    // Main loop
    void run();
    
    // Message handling
    void sendMessage(const std::string& message);
    
private:
    // Network
    void startRead();
    void handleMessage(const std::string& message);
    void processMessages();
    
    // User input
    void handleUserInput();
    bool checkInputAvailable();
    void processCommand(const std::string& command, const std::vector<std::string>& args);
    
    // Game state
    void updateGameState(const GameState& newState);
    void displayCurrentState();
    
    // Specific command handlers
    void handleSelectCommand(const std::vector<std::string>& args);
    void handlePlaceCommand(const std::vector<std::string>& args);
    void handleMoveCommand(const std::vector<std::string>& args);
    void handleSkillCommand(const std::vector<std::string>& args);
    void handlePassCommand(const std::vector<std::string>& args);
    void handleReadyCommand();
    void handleViewCommand();
    void handleHelpCommand();
    
    // Network members
    boost::asio::io_context ioContext_;
    std::unique_ptr<tcp::socket> socket_;
    boost::asio::streambuf readBuffer_;
    std::string messageBuffer_;
    std::thread ioThread_;
    
    // Game state
    GameState gameState_;
    int myPlayerId_;
    bool connected_;
    bool running_;
    
    // UI
    TextRenderer renderer_;
    InputHandler inputHandler_;
    MenuSystem menuSystem_;
};
