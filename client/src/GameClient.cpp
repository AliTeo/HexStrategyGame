#include "GameClient.h"
#include <iostream>
#include <sys/select.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>

GameClient::GameClient()
    : myPlayerId_(-1), connected_(false), running_(false) {}

GameClient::~GameClient() {
    disconnect();
}

bool GameClient::connect(const std::string& host, const std::string& port) {
    try {
        socket_ = std::make_unique<tcp::socket>(ioContext_);
        
        tcp::resolver resolver(ioContext_);
        auto endpoints = resolver.resolve(host, port);
        
        boost::asio::connect(*socket_, endpoints);
        
        connected_ = true;
        std::cout << "Connected to server at " << host << ":" << port << std::endl;
        
        // Start reading messages in background thread
        ioThread_ = std::thread([this]() {
            try {
                ioContext_.run();
            } catch (std::exception& e) {
                std::cerr << "IO error: " << e.what() << std::endl;
                connected_ = false;
            }
        });
        
        startRead();
        return true;
        
    } catch (std::exception& e) {
        std::cerr << "Connection failed: " << e.what() << std::endl;
        return false;
    }
}

void GameClient::disconnect() {
    if (connected_) {
        connected_ = false;
        running_ = false;
        
        if (socket_) {
            boost::system::error_code ec;
            socket_->close(ec);
        }
        
        ioContext_.stop();
        
        if (ioThread_.joinable()) {
            ioThread_.join();
        }
    }
}

void GameClient::run() {
    running_ = true;
    renderer_.displayLogo();
    renderer_.displayWelcome();
    
    // Give server time to send initial messages
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // Display initial prompt
    std::string prompt = "> ";
    std::cout << prompt << std::flush;
    
    while (running_ && connected_) {
        // Process incoming messages (non-blocking)
        processMessages();
        
        // Check if input is available (non-blocking)
        if (checkInputAvailable()) {
            handleUserInput();
            
            // Redisplay prompt after processing command
            switch (gameState_.phase) {
                case GamePhase::SELECTION:
                    prompt = "[SELECT] > ";
                    break;
                case GamePhase::PLACEMENT:
                    prompt = "[PLACE] > ";
                    break;
                case GamePhase::PLAYING:
                    prompt = "[BATTLE] > ";
                    break;
                default:
                    prompt = "> ";
                    break;
            }
            std::cout << prompt << std::flush;
        }
        
        // Small sleep to prevent CPU spinning
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

void GameClient::startRead() {
    boost::asio::async_read_until(*socket_, readBuffer_, Protocol::MESSAGE_DELIMITER,
        [this](const boost::system::error_code& error, std::size_t bytes_transferred) {
            if (!error) {
                std::string data{
                    boost::asio::buffers_begin(readBuffer_.data()),
                    boost::asio::buffers_begin(readBuffer_.data()) + bytes_transferred
                };
                readBuffer_.consume(bytes_transferred);
                
                std::cerr << "[DEBUG] Received " << bytes_transferred << " bytes" << std::endl;
                messageBuffer_ += data;
                
                startRead();
            } else {
                std::cerr << "\nConnection lost: " << error.message() << std::endl;
                connected_ = false;
                running_ = false;
            }
        });
}

void GameClient::sendMessage(const std::string& message) {
    if (!connected_) return;
    
    std::string fullMessage = Protocol::addDelimiter(message);
    
    boost::asio::async_write(*socket_, boost::asio::buffer(fullMessage),
        [](const boost::system::error_code& error, std::size_t /*bytes_transferred*/) {
            if (error) {
                std::cerr << "Send error: " << error.message() << std::endl;
            }
        });
}

void GameClient::processMessages() {
    auto messages = Protocol::extractMessages(messageBuffer_);
    
    if (!messages.empty()) {
        std::cerr << "[DEBUG] Processing " << messages.size() << " messages" << std::endl;
    }
    
    for (const auto& message : messages) {
        handleMessage(message);
    }
}

void GameClient::handleMessage(const std::string& message) {
    MessageType type;
    nlohmann::json data;
    
    if (!Protocol::parseMessage(message, type, data)) {
        std::cerr << "[DEBUG] Failed to parse message" << std::endl;
        return;
    }
    
    std::cerr << "[DEBUG] Handling message type: " << static_cast<int>(type) << std::endl;
    
    // Clear the current prompt line for clean output
    std::cout << "\r" << std::string(80, ' ') << "\r";
    
    switch (type) {
        case MessageType::CONNECTED:
            myPlayerId_ = data["playerId"];
            std::cout << "✓ Connected as Player " << myPlayerId_ << std::endl;
            std::cout << "Waiting for opponent..." << std::endl;
            break;
            
        case MessageType::PLAYER_JOINED:
            std::cout << "✓ Player " << data["playerId"].get<int>() << " joined" << std::endl;
            break;
            
        case MessageType::GAME_STARTED:
            std::cout << "✓ Game starting! Both players connected." << std::endl;
            std::cout << "Waiting for game state..." << std::endl;
            break;
            
        case MessageType::GAME_STATE_UPDATE: {
            GameState newState = GameState::fromJson(data);
            updateGameState(newState);
            break;
        }
        
        case MessageType::TURN_RESOLVED:
            renderer_.displayTurnSummary(data["summary"]);
            std::cout << "New turn begins!" << std::endl;
            break;
            
        case MessageType::ERROR_MSG:
            renderer_.displayError(data["error"]);
            break;
            
        default:
            break;
    }
}

void GameClient::updateGameState(const GameState& newState) {
    GamePhase oldPhase = gameState_.phase;
    gameState_ = newState;
    
    // Notify phase changes
    if (oldPhase != newState.phase) {
        switch (newState.phase) {
            case GamePhase::SELECTION:
                std::cout << "==========================================" << std::endl;
                std::cout << "     CHARACTER SELECTION PHASE" << std::endl;
                std::cout << "==========================================" << std::endl;
                renderer_.displayAvailableClasses();
                std::cout << "Use 'select <0-5>' to choose a character (need 3 total)" << std::endl;
                std::cout << "Waiting for your selection..." << std::endl;
                break;
                
            case GamePhase::PLACEMENT:
                std::cout << "==========================================" << std::endl;
                std::cout << "     CHARACTER PLACEMENT PHASE" << std::endl;
                std::cout << "==========================================" << std::endl;
                std::cout << "Place your 3 characters on your side of the grid" << std::endl;
                if (myPlayerId_ == 0) {
                    std::cout << "You are Player 0: place on LEFT side (q < 0)" << std::endl;
                } else {
                    std::cout << "You are Player 1: place on RIGHT side (q > 0)" << std::endl;
                }
                std::cout << "Use 'place <class_id> <q> <r>' to place a character" << std::endl;
                renderer_.displayHexGrid(gameState_, myPlayerId_);
                displayCurrentState();
                std::cout << "Waiting for placements..." << std::endl;
                break;
                
            case GamePhase::PLAYING:
                std::cout << "==========================================" << std::endl;
                std::cout << "         BATTLE PHASE - FIGHT!" << std::endl;
                std::cout << "==========================================" << std::endl;
                displayCurrentState();
                std::cout << "Enter your commands..." << std::endl;
                break;
                
            case GamePhase::GAME_OVER:
                std::cout << "=== GAME OVER ===" << std::endl;
                if (gameState_.winnerId == myPlayerId_) {
                    std::cout << "🎉 YOU WIN! 🎉" << std::endl;
                } else {
                    std::cout << "You lose. Better luck next time!" << std::endl;
                }
                running_ = false;
                break;
                
            default:
                break;
        }
    } else {
        // Same phase, just update display
        displayCurrentState();
    }
}

void GameClient::displayCurrentState() {
    renderer_.displayGameState(gameState_, myPlayerId_);
    renderer_.displayHexGrid(gameState_, myPlayerId_);
    renderer_.displayCharacterList(gameState_, myPlayerId_);
}

bool GameClient::checkInputAvailable() {
    fd_set readfds;
    struct timeval tv;
    
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);
    
    // Set timeout to 0 for non-blocking check
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    
    int result = select(STDIN_FILENO + 1, &readfds, nullptr, nullptr, &tv);
    return result > 0 && FD_ISSET(STDIN_FILENO, &readfds);
}

void GameClient::handleUserInput() {
    std::string input;
    std::getline(std::cin, input);
    
    std::string command;
    std::vector<std::string> args;
    
    if (!inputHandler_.parseInput(input, command, args)) {
        return;
    }
    
    processCommand(command, args);
}

void GameClient::processCommand(const std::string& command, const std::vector<std::string>& args) {
    if (command == "select") {
        handleSelectCommand(args);
    } else if (command == "place") {
        handlePlaceCommand(args);
    } else if (command == "move") {
        handleMoveCommand(args);
    } else if (command == "skill") {
        handleSkillCommand(args);
    } else if (command == "pass") {
        handlePassCommand(args);
    } else if (command == "ready") {
        handleReadyCommand();
    } else if (command == "view") {
        handleViewCommand();
    } else if (command == "help") {
        handleHelpCommand();
    } else if (command == "quit") {
        running_ = false;
    } else {
        std::cout << "Unknown command. Type 'help' for available commands.\n";
    }
}

void GameClient::handleSelectCommand(const std::vector<std::string>& args) {
    int classId;
    if (!inputHandler_.parseSelectCommand(args, classId)) {
        renderer_.displayError("Usage: select <0-5>");
        return;
    }
    
    CharacterClass charClass = static_cast<CharacterClass>(classId);
    sendMessage(Protocol::selectCharacterMessage(myPlayerId_, charClass));
}

void GameClient::handlePlaceCommand(const std::vector<std::string>& args) {
    int classId, q, r;
    if (!inputHandler_.parsePlaceCommand(args, classId, q, r)) {
        renderer_.displayError("Usage: place <class_id> <q> <r>");
        return;
    }
    
    CharacterClass charClass = static_cast<CharacterClass>(classId);
    HexCoord pos(q, r);
    sendMessage(Protocol::placeCharacterMessage(myPlayerId_, charClass, pos));
}

void GameClient::handleMoveCommand(const std::vector<std::string>& args) {
    std::string charId;
    int q, r;
    if (!inputHandler_.parseMoveCommand(args, charId, q, r)) {
        renderer_.displayError("Usage: move <char_id> <q> <r>");
        return;
    }
    
    Action action(ActionType::MOVE, charId, HexCoord(q, r));
    sendMessage(Protocol::submitActionMessage(action));
    std::cout << "Move action submitted for " << charId << "\n";
}

void GameClient::handleSkillCommand(const std::vector<std::string>& args) {
    std::string charId, skillId;
    int q, r;
    if (!inputHandler_.parseSkillCommand(args, charId, skillId, q, r)) {
        renderer_.displayError("Usage: skill <char_id> <skill_id> <q> <r>");
        return;
    }
    
    Action action(charId, skillId, HexCoord(q, r));
    sendMessage(Protocol::submitActionMessage(action));
    std::cout << "Skill action submitted for " << charId << "\n";
}

void GameClient::handlePassCommand(const std::vector<std::string>& args) {
    std::string charId;
    if (!inputHandler_.parsePassCommand(args, charId)) {
        renderer_.displayError("Usage: pass <char_id>");
        return;
    }
    
    Action action(ActionType::PASS, charId);
    sendMessage(Protocol::submitActionMessage(action));
    std::cout << charId << " will pass this turn\n";
}

void GameClient::handleReadyCommand() {
    sendMessage(Protocol::lockTurnMessage(myPlayerId_));
    std::cout << "Turn locked in. Waiting for opponent...\n";
}

void GameClient::handleViewCommand() {
    displayCurrentState();
}

void GameClient::handleHelpCommand() {
    renderer_.displayHelp();
}
