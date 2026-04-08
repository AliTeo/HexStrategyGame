#include "SFMLGameClient.h"
#include <iostream>

SFMLGameClient::SFMLGameClient()
    : connected_(false), running_(false), myPlayerId_(-1),
      currentPlacementIndex_(0), battleMode_(BattleMode::NONE) {
}

SFMLGameClient::~SFMLGameClient() {
    disconnect();
}

bool SFMLGameClient::connect(const std::string& host, const std::string& port) {
    try {
        socket_ = std::make_unique<tcp::socket>(ioContext_);
        
        tcp::resolver resolver(ioContext_);
        auto endpoints = resolver.resolve(host, port);
        
        boost::asio::connect(*socket_, endpoints);
        
        connected_ = true;
        std::cout << "Connected to server at " << host << ":" << port << std::endl;
        
        // Start IO thread
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

void SFMLGameClient::disconnect() {
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

void SFMLGameClient::startRead() {
    boost::asio::async_read_until(*socket_, readBuffer_, Protocol::MESSAGE_DELIMITER,
        [this](const boost::system::error_code& error, std::size_t bytes_transferred) {
            if (!error) {
                std::string data{
                    boost::asio::buffers_begin(readBuffer_.data()),
                    boost::asio::buffers_begin(readBuffer_.data()) + bytes_transferred
                };
                readBuffer_.consume(bytes_transferred);
                
                messageBuffer_ += data;
                startRead();
            } else {
                std::cerr << "Connection lost: " << error.message() << std::endl;
                connected_ = false;
                running_ = false;
            }
        });
}

void SFMLGameClient::sendMessage(const std::string& message) {
    if (!connected_) return;
    
    std::string fullMessage = Protocol::addDelimiter(message);
    
    boost::asio::async_write(*socket_, boost::asio::buffer(fullMessage),
        [](const boost::system::error_code& error, std::size_t) {
            if (error) {
                std::cerr << "Send error: " << error.message() << std::endl;
            }
        });
}

void SFMLGameClient::processMessages() {
    auto messages = Protocol::extractMessages(messageBuffer_);
    
    for (const auto& message : messages) {
        handleMessage(message);
    }
}

void SFMLGameClient::handleMessage(const std::string& message) {
    MessageType type;
    nlohmann::json data;
    
    if (!Protocol::parseMessage(message, type, data)) {
        return;
    }
    
    switch (type) {
        case MessageType::CONNECTED:
            myPlayerId_ = data["playerId"];
            std::cout << "Connected as Player " << myPlayerId_ << std::endl;
            break;
            
        case MessageType::PLAYER_JOINED:
            std::cout << "Player " << data["playerId"].get<int>() << " joined" << std::endl;
            break;
            
        case MessageType::GAME_STARTED:
            std::cout << "Game starting!" << std::endl;
            break;
            
        case MessageType::GAME_STATE_UPDATE: {
            GameState newState = GameState::fromJson(data);
            updateGameState(newState);
            break;
        }
        
        case MessageType::TURN_RESOLVED:
            std::cout << "Turn resolved!" << std::endl;
            break;
            
        case MessageType::ERROR_MSG:
            std::cerr << "Server error: " << data["error"].get<std::string>() << std::endl;
            break;
            
        default:
            break;
    }
}

void SFMLGameClient::updateGameState(const GameState& newState) {
    GamePhase oldPhase = gameState_.phase;
    
    // Track our placed character count before update
    int oldPlacedCount = 0;
    if (gameState_.phase == GamePhase::PLACEMENT && myPlayerId_ >= 0) {
        for (const auto& [pos, charId] : gameState_.occupancy) {
            if (gameState_.characters.count(charId) && 
                gameState_.characters.at(charId)->playerId == myPlayerId_) {
                oldPlacedCount++;
            }
        }
    }
    
    gameState_ = newState;
    
    // Check if we placed a new character
    if (newState.phase == GamePhase::PLACEMENT && myPlayerId_ >= 0) {
        int newPlacedCount = 0;
        for (const auto& [pos, charId] : newState.occupancy) {
            if (newState.characters.count(charId) && 
                newState.characters.at(charId)->playerId == myPlayerId_) {
                newPlacedCount++;
            }
        }
        
        // If we placed a new character, advance to next
        if (newPlacedCount > oldPlacedCount) {
            currentPlacementIndex_++;
            std::cout << "Character placed! Next character: " << currentPlacementIndex_ << std::endl;
        }
    }
    
    if (oldPhase != newState.phase) {
        switch (newState.phase) {
            case GamePhase::SELECTION:
                std::cout << "=== CHARACTER SELECTION PHASE ===" << std::endl;
                selectedClasses_.clear();
                break;
                
            case GamePhase::PLACEMENT:
                std::cout << "=== CHARACTER PLACEMENT PHASE ===" << std::endl;
                currentPlacementIndex_ = 0;  // Reset to first character
                break;
                
            case GamePhase::PLAYING:
                std::cout << "=== BATTLE PHASE ===" << std::endl;
                clearSelection();
                pendingActions_.clear();
                break;
                
            case GamePhase::GAME_OVER:
                std::cout << "=== GAME OVER ===" << std::endl;
                if (gameState_.winnerId == myPlayerId_) {
                    std::cout << "YOU WIN!" << std::endl;
                } else {
                    std::cout << "You lose!" << std::endl;
                }
                running_ = false;
                break;
                
            default:
                break;
        }
    }
}

void SFMLGameClient::run() {
    sf::RenderWindow window(sf::VideoMode({1024, 768}), "Hexagonal Strategy Game");
    window.setFramerateLimit(60);
    
    running_ = true;
    renderer_.setGridCenter(sf::Vector2f(512.0f, 384.0f));
    
    while (window.isOpen() && running_ && connected_) {
        // Process network messages
        processMessages();
        
        // Handle events
        while (std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
                running_ = false;
            }
            
            if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mousePressed->button == sf::Mouse::Button::Left) {
                    handleMouseClick(mousePressed->position, window);
                }
            }
            
            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                handleKeyPress(keyPressed->code);
            }
        }
        
        // Render
        window.clear(sf::Color(20, 20, 30));
        
        switch (gameState_.phase) {
            case GamePhase::WAITING:
                // Draw waiting screen
                break;
                
            case GamePhase::SELECTION:
                handleSelectionPhase(window);
                break;
                
            case GamePhase::PLACEMENT:
                handlePlacementPhase(window);
                break;
                
            case GamePhase::PLAYING:
                handlePlayingPhase(window);
                break;
                
            case GamePhase::GAME_OVER:
                renderer_.drawHexGrid(window, gameState_, myPlayerId_);
                // Draw game over screen
                break;
                
            default:
                break;
        }
        
        window.display();
    }
}

void SFMLGameClient::handleSelectionPhase(sf::RenderWindow& window) {
    renderer_.drawSelectionScreen(window, selectedClasses_);
}

void SFMLGameClient::handlePlacementPhase(sf::RenderWindow& window) {
    renderer_.drawPlacementScreen(window, gameState_, myPlayerId_);
}

void SFMLGameClient::handlePlayingPhase(sf::RenderWindow& window) {
    renderer_.drawHexGrid(window, gameState_, myPlayerId_);
    
    // Draw pending actions (arrows)
    renderer_.drawPendingActions(window, gameState_, pendingActions_);
    
    // Draw selection highlight if character is selected
    if (!selectedCharacterId_.empty() && gameState_.characters.count(selectedCharacterId_)) {
        for (const auto& [pos, charId] : gameState_.occupancy) {
            if (charId == selectedCharacterId_) {
                renderer_.drawSelectionHighlight(window, pos);
                break;
            }
        }
    }
    
    renderer_.drawUI(window, gameState_, myPlayerId_);
}

void SFMLGameClient::handleMouseClick(sf::Vector2i mousePos, sf::RenderWindow& window) {
    (void)window;  // Suppress unused warning
    sf::Vector2f worldPos(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
    
    switch (gameState_.phase) {
        case GamePhase::SELECTION: {
            // Determine which class was clicked (simple grid layout)
            int boxWidth = 220;
            int boxHeight = 180;
            int startX = 100;
            int startY = 200;
            
            for (int i = 0; i < 6; ++i) {
                int x = startX + (i % 3) * boxWidth;
                int y = startY + (i / 3) * boxHeight;
                
                if (mousePos.x >= x && mousePos.x <= x + 200 &&
                    mousePos.y >= y && mousePos.y <= y + 150) {
                    selectCharacter(static_cast<CharacterClass>(i));
                    break;
                }
            }
            break;
        }
        
        case GamePhase::PLACEMENT: {
            HexCoord clickedHex = renderer_.pixelToHex(worldPos);
            
            // If we have selected classes, use currentPlacementIndex_ to cycle through
            if (!gameState_.selectedClasses.empty() && 
                gameState_.selectedClasses.count(myPlayerId_)) {
                
                auto& myClasses = gameState_.selectedClasses.at(myPlayerId_);
                std::cout << "[CLIENT] currentPlacementIndex_=" << currentPlacementIndex_ 
                          << " myClasses.size()=" << myClasses.size() << std::endl;
                
                if (currentPlacementIndex_ < myClasses.size()) {
                    CharacterClass classToPlace = myClasses[currentPlacementIndex_];
                    std::cout << "[CLIENT] Placing class " << static_cast<int>(classToPlace) 
                              << " at (" << clickedHex.q << "," << clickedHex.r << ")" << std::endl;
                    placeCharacter(classToPlace, clickedHex);
                    // After successful placement, advance to next character
                    // (placeCharacter will handle success/failure)
                } else {
                    std::cout << "[CLIENT] All characters placed!" << std::endl;
                }
            }
            break;
        }
        
        case GamePhase::PLAYING: {
            // Handle battle phase clicks
            HexCoord clickedHex = renderer_.pixelToHex(worldPos);
            
            if (battleMode_ == BattleMode::NONE) {
                // No mode selected, so clicking should select a character
                Character* clickedChar = findCharacterAtHex(clickedHex);
                if (clickedChar && clickedChar->playerId == myPlayerId_ && clickedChar->isAlive) {
                    selectCharacterForCommand(clickedChar->id);
                }
            } else {
                // Mode is selected (MOVE or ATTACK), execute command
                executeCommand(clickedHex);
            }
            break;
        }
        
        default:
            break;
    }
}

void SFMLGameClient::handleKeyPress(sf::Keyboard::Key key) {
    switch (key) {
        case sf::Keyboard::Key::Escape:
            running_ = false;
            break;
            
        case sf::Keyboard::Key::Space:
            if (gameState_.phase == GamePhase::PLAYING) {
                lockTurn();
            }
            break;
            
        case sf::Keyboard::Key::M:
            if (gameState_.phase == GamePhase::PLAYING && !selectedCharacterId_.empty()) {
                battleMode_ = BattleMode::MOVE;
                std::cout << "Mode: MOVE - Click a hex to move to" << std::endl;
            }
            break;
            
        case sf::Keyboard::Key::A:
            if (gameState_.phase == GamePhase::PLAYING && !selectedCharacterId_.empty()) {
                battleMode_ = BattleMode::ATTACK;
                std::cout << "Mode: ATTACK - Click a hex to attack" << std::endl;
            }
            break;
            
        default:
            break;
    }
}

void SFMLGameClient::selectCharacter(CharacterClass charClass) {
    std::cout << "Selecting class: " << static_cast<int>(charClass) << std::endl;
    sendMessage(Protocol::selectCharacterMessage(myPlayerId_, charClass));
    selectedClasses_.push_back(charClass);
}

void SFMLGameClient::placeCharacter(CharacterClass charClass, const HexCoord& pos) {
    std::cout << "Placing character class " << static_cast<int>(charClass) 
              << " at (" << pos.q << ", " << pos.r << ")" << std::endl;
    sendMessage(Protocol::placeCharacterMessage(myPlayerId_, charClass, pos));
}

void SFMLGameClient::submitAction(const Action& action) {
    sendMessage(Protocol::submitActionMessage(action));
}

void SFMLGameClient::lockTurn() {
    std::cout << "Locking turn..." << std::endl;
    
    // Submit all pending actions
    for (const auto& action : pendingActions_) {
        submitAction(action);
    }
    
    // Clear pending actions and selection
    pendingActions_.clear();
    clearSelection();
    
    // Lock the turn
    sendMessage(Protocol::lockTurnMessage(myPlayerId_));
}

void SFMLGameClient::clearSelection() {
    selectedCharacterId_.clear();
    battleMode_ = BattleMode::NONE;
}

void SFMLGameClient::selectCharacterForCommand(const std::string& characterId) {
    selectedCharacterId_ = characterId;
    battleMode_ = BattleMode::NONE;
    std::cout << "Selected character: " << characterId << std::endl;
    std::cout << "Press 'M' for move mode, 'A' for attack mode" << std::endl;
}

bool SFMLGameClient::hasActionForCharacter(const std::string& characterId) const {
    for (const auto& action : pendingActions_) {
        if (action.characterId == characterId) {
            return true;
        }
    }
    return false;
}

Character* SFMLGameClient::findCharacterAtHex(const HexCoord& hex) {
    auto it = gameState_.occupancy.find(hex);
    if (it != gameState_.occupancy.end() && gameState_.characters.count(it->second)) {
        return gameState_.characters[it->second].get();
    }
    return nullptr;
}

void SFMLGameClient::executeCommand(const HexCoord& targetHex) {
    if (selectedCharacterId_.empty() || battleMode_ == BattleMode::NONE) {
        return;
    }
    
    // Check if this character already has an action
    if (hasActionForCharacter(selectedCharacterId_)) {
        std::cout << "Character " << selectedCharacterId_ << " already has an action assigned!" << std::endl;
        std::cout << "Clear it first if you want to change." << std::endl;
        return;
    }
    
    Action action;
    action.characterId = selectedCharacterId_;
    
    if (battleMode_ == BattleMode::MOVE) {
        action.type = ActionType::MOVE;
        action.target = targetHex;
        std::cout << "Queued MOVE for " << selectedCharacterId_ 
                  << " to (" << targetHex.q << ", " << targetHex.r << ")" << std::endl;
    } else if (battleMode_ == BattleMode::ATTACK) {
        // Find character's first damaging skill
        if (gameState_.characters.count(selectedCharacterId_)) {
            auto& character = gameState_.characters[selectedCharacterId_];
            bool foundSkill = false;
            
            for (const auto& skill : character->skills) {
                if (skill.type == SkillType::DAMAGE) {
                    action.type = ActionType::USE_SKILL;
                    action.skillId = skill.id;
                    action.target = targetHex;
                    std::cout << "Queued SKILL " << skill.id << " for " << selectedCharacterId_ 
                              << " targeting (" << targetHex.q << ", " << targetHex.r << ")" << std::endl;
                    foundSkill = true;
                    break;
                }
            }
            
            if (!foundSkill) {
                std::cout << "No attack skill found for this character!" << std::endl;
                return;
            }
        }
    }
    
    pendingActions_.push_back(action);
    clearSelection();
}

