#include "SFMLGameClient.h"
#include <algorithm>
#include <iostream>
#include <string_view>

SFMLGameClient::SFMLGameClient()
    : connected_(false), running_(false), gameStarted_(false), myPlayerId_(-1),
      currentPlacementIndex_(0), lobbyStatus_("Not connected"),
      selectionElapsedSeconds_(0.0f), selectionFlashUntilSeconds_(0.0f),
      selectionReadySent_(false), readyButtonHovered_(false),
      postShaderLoaded_(false),
      battleMode_(BattleMode::NONE) {
}

SFMLGameClient::~SFMLGameClient() {
    disconnect();
}

bool SFMLGameClient::connect(const std::string& host, const std::string& port) {
    try {
        ioContext_.restart();
        socket_ = std::make_unique<tcp::socket>(ioContext_);
        
        tcp::resolver resolver(ioContext_);
        auto endpoints = resolver.resolve(host, port);
        
        boost::asio::connect(*socket_, endpoints);
        
        connected_ = true;
        gameStarted_ = false;
        setLobbyStatus("Found server. Waiting for opponent...");
        std::cout << "Connected to server at " << host << ":" << port << std::endl;
        
        // Start IO thread
        ioThread_ = std::thread([this]() {
            try {
                ioContext_.run();
            } catch (std::exception& e) {
                std::cerr << "IO error: " << e.what() << std::endl;
                connected_ = false;
                setLobbyStatus("Connection lost");
            }
        });
        
        startRead();
        return true;
        
    } catch (std::exception& e) {
        std::cerr << "Connection failed: " << e.what() << std::endl;
        setLobbyStatus("Unable to find server");
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
    gameStarted_ = false;
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
                
                {
                    std::lock_guard<std::mutex> lock(messageMutex_);
                    messageBuffer_ += data;
                }
                startRead();
            } else {
                std::cerr << "Connection lost: " << error.message() << std::endl;
                connected_ = false;
                running_ = false;
                setLobbyStatus("Connection lost");
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
    std::string pendingBuffer;
    {
        std::lock_guard<std::mutex> lock(messageMutex_);
        pendingBuffer.swap(messageBuffer_);
    }
    auto messages = Protocol::extractMessages(pendingBuffer);
    if (!pendingBuffer.empty()) {
        std::lock_guard<std::mutex> lock(messageMutex_);
        messageBuffer_.insert(0, pendingBuffer);
    }
    
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
            setLobbyStatus("Found server. Waiting for opponent...");
            std::cout << "Connected as Player " << myPlayerId_ << std::endl;
            pushUiInfo("Connected as Player " + std::to_string(myPlayerId_));
            break;
            
        case MessageType::PLAYER_JOINED:
            setLobbyStatus("Found server. Waiting for opponent...");
            std::cout << "Player " << data["playerId"].get<int>() << " joined" << std::endl;
            break;
            
        case MessageType::GAME_STARTED:
            gameStarted_ = true;
            setLobbyStatus("Opponent ready. Starting game...");
            std::cout << "Game starting!" << std::endl;
            pushUiInfo("Game starting.");
            break;
            
        case MessageType::GAME_STATE_UPDATE: {
            GameState newState = GameState::fromJson(data);
            updateGameState(newState);
            break;
        }
        
        case MessageType::TURN_RESOLVED:
            std::cout << "Turn resolved!" << std::endl;
            pushUiInfo("Turn resolved.");
            break;
            
        case MessageType::ERROR_MSG:
            setLobbyStatus(data["error"].get<std::string>());
            std::cerr << "Server error: " << data["error"].get<std::string>() << std::endl;
            pushUiInfo("Server: " + data["error"].get<std::string>());
            break;
            
        default:
            break;
    }
}

void SFMLGameClient::pumpNetwork() {
    processMessages();
}

bool SFMLGameClient::isConnected() const noexcept {
    return connected_;
}

bool SFMLGameClient::hasGameStarted() const noexcept {
    return gameStarted_;
}

std::string SFMLGameClient::getLobbyStatus() const {
    std::lock_guard<std::mutex> lock(statusMutex_);
    return lobbyStatus_;
}

void SFMLGameClient::setLobbyStatus(std::string status) {
    std::lock_guard<std::mutex> lock(statusMutex_);
    lobbyStatus_ = std::move(status);
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

    selectedClasses_.clear();
    if (myPlayerId_ >= 0 && newState.selectedClasses.count(myPlayerId_)) {
        selectedClasses_ = newState.selectedClasses.at(myPlayerId_);
    }
    if (myPlayerId_ >= 0 && newState.playerReady.count(myPlayerId_)) {
        selectionReadySent_ = newState.playerReady.at(myPlayerId_);
    }
    
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
                pushUiInfo("Selection phase.");
                selectedClasses_.clear();
                hoveredSelectionClass_.reset();
                recentlySelectedClass_.reset();
                selectionReadySent_ = false;
                readyButtonHovered_ = false;
                break;
                
            case GamePhase::PLACEMENT:
                std::cout << "=== CHARACTER PLACEMENT PHASE ===" << std::endl;
                pushUiInfo("Placement phase: place 3 characters.");
                currentPlacementIndex_ = 0;  // Reset to first character
                break;
                
            case GamePhase::PLAYING:
                std::cout << "=== BATTLE PHASE ===" << std::endl;
                pushUiInfo("Battle phase started.");
                clearSelection();
                pendingActions_.clear();
                break;
                
            case GamePhase::GAME_OVER:
                std::cout << "=== GAME OVER ===" << std::endl;
                if (gameState_.winnerId == myPlayerId_) {
                    std::cout << "YOU WIN!" << std::endl;
                    pushUiInfo("Game over: You win!");
                } else {
                    std::cout << "You lose!" << std::endl;
                    pushUiInfo("Game over: You lose.");
                }
                running_ = false;
                break;
                
            default:
                break;
        }
    }
}

void SFMLGameClient::run() {
    sf::ContextSettings settings;
    settings.antiAliasingLevel = std::min(8u, sf::RenderTexture::getMaximumAntiAliasingLevel());
    sf::RenderWindow window(sf::VideoMode({1024, 768}),
                            "Hexagonal Strategy Game",
                            sf::Style::Default,
                            sf::State::Windowed,
                            settings);
    window.setVerticalSyncEnabled(true);
    sf::Clock renderClock;

    if (!sceneTexture_.resize(window.getSize(), settings)) {
        postShaderLoaded_ = false;
    }
    sceneTexture_.setSmooth(true);

    if (sf::Shader::isAvailable()) {
        constexpr std::string_view kPostFragment = R"(
            uniform sampler2D texture;
            uniform float time;
            uniform vec2 texelSize;
            uniform float phaseStrength;
            void main() {
                vec2 uv = gl_TexCoord[0].xy;
                vec4 center = texture2D(texture, uv);
                vec4 blur = (
                    texture2D(texture, uv + vec2(texelSize.x, 0.0)) +
                    texture2D(texture, uv - vec2(texelSize.x, 0.0)) +
                    texture2D(texture, uv + vec2(0.0, texelSize.y)) +
                    texture2D(texture, uv - vec2(0.0, texelSize.y))
                ) * 0.25;

                vec4 color = mix(center, blur, 0.10 + phaseStrength * 0.05);
                float edgeDist = length((uv - 0.5) * 1.20);
                float vignette = smoothstep(1.20, 0.26, edgeDist);
                float pulse = 0.984 + 0.016 * sin(time * 1.5);
                color.rgb *= vignette * pulse;

                float contrast = 1.08 + phaseStrength * 0.08;
                color.rgb = (color.rgb - 0.5) * contrast + 0.5;

                float luma = dot(color.rgb, vec3(0.299, 0.587, 0.114));
                float saturation = 1.04 + phaseStrength * 0.08;
                color.rgb = mix(vec3(luma), color.rgb, saturation);
                color.rgb = pow(color.rgb, vec3(0.92));
                gl_FragColor = color;
            }
        )";
        postShaderLoaded_ = postShader_.loadFromMemory(kPostFragment, sf::Shader::Type::Fragment);
        if (postShaderLoaded_) {
            postShader_.setUniform("texture", sf::Shader::CurrentTexture);
            const auto size = window.getSize();
            postShader_.setUniform("texelSize", sf::Glsl::Vec2(1.f / static_cast<float>(size.x), 1.f / static_cast<float>(size.y)));
        }
    } else {
        postShaderLoaded_ = false;
    }
    
    running_ = true;
    renderer_.setGridCenter(sf::Vector2f(512.0f, 384.0f));
    
    while (window.isOpen() && running_ && connected_) {
        selectionElapsedSeconds_ = renderClock.getElapsedTime().asSeconds();

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

            if (const auto* mouseMoved = event->getIf<sf::Event::MouseMoved>()) {
                handleMouseMove(mouseMoved->position);
            }
            
            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                handleKeyPress(keyPressed->code);
            }
        }
        
        // Render into offscreen texture first
        sceneTexture_.clear(sf::Color(20, 20, 30));
        
        switch (gameState_.phase) {
            case GamePhase::WAITING:
                // Draw waiting screen
                break;
                
            case GamePhase::SELECTION:
                handleSelectionPhase(sceneTexture_);
                break;
                
            case GamePhase::PLACEMENT:
                handlePlacementPhase(sceneTexture_);
                break;
                
            case GamePhase::PLAYING:
                handlePlayingPhase(sceneTexture_);
                break;
                
            case GamePhase::GAME_OVER:
                renderer_.drawHexGrid(sceneTexture_, gameState_, myPlayerId_);
                renderer_.drawUI(sceneTexture_, gameState_, myPlayerId_, uiInfoLines_);
                // Draw game over screen
                break;
                
            default:
                break;
        }

        sceneTexture_.display();
        window.clear(sf::Color(10, 12, 16));
        sf::Sprite sceneSprite(sceneTexture_.getTexture());
        if (postShaderLoaded_) {
            postShader_.setUniform("time", selectionElapsedSeconds_);
            const bool isBattle = (gameState_.phase == GamePhase::PLAYING);
            postShader_.setUniform("phaseStrength", isBattle ? 1.15f : 0.82f);
            sf::RenderStates states;
            states.shader = &postShader_;
            window.draw(sceneSprite, states);
        } else {
            window.draw(sceneSprite);
        }
        window.display();
    }
}

void SFMLGameClient::handleSelectionPhase(sf::RenderTarget& window) {
    bool opponentReady = false;
    if (myPlayerId_ == 0 && gameState_.playerReady.count(1)) {
        opponentReady = gameState_.playerReady.at(1);
    } else if (myPlayerId_ == 1 && gameState_.playerReady.count(0)) {
        opponentReady = gameState_.playerReady.at(0);
    }
    const bool readyEnabled = selectedClasses_.size() == static_cast<std::size_t>(GameState::CHARS_PER_PLAYER);
    if (selectionFlashUntilSeconds_ < selectionElapsedSeconds_) {
        recentlySelectedClass_.reset();
    }

    renderer_.drawSelectionScreen(window,
                                  selectedClasses_,
                                  hoveredSelectionClass_,
                                  readyEnabled,
                                  selectionReadySent_,
                                  readyButtonHovered_,
                                  selectionElapsedSeconds_,
                                  recentlySelectedClass_,
                                  opponentReady);
}

void SFMLGameClient::handlePlacementPhase(sf::RenderTarget& window) {
    renderer_.drawPlacementScreen(window, gameState_, myPlayerId_);
    renderer_.drawUI(window, gameState_, myPlayerId_, uiInfoLines_);
}

void SFMLGameClient::handlePlayingPhase(sf::RenderTarget& window) {
    renderer_.drawHexGrid(window, gameState_, myPlayerId_);
    
    // Draw pending actions (arrows)
    renderer_.drawPendingActions(window, gameState_, pendingActions_);
    
    // Draw selection highlight and ranges if character is selected
    if (!selectedCharacterId_.empty() && gameState_.characters.count(selectedCharacterId_)) {
        for (const auto& [pos, charId] : gameState_.occupancy) {
            if (charId == selectedCharacterId_) {
                renderer_.drawSelectionHighlight(window, pos);
                const Character& selected = *gameState_.characters.at(selectedCharacterId_);
                int maxAttackRange = 1;
                for (const Skill& skill : selected.skills) {
                    maxAttackRange = std::max(maxAttackRange, skill.range);
                }
                const bool showMove = (battleMode_ != BattleMode::ATTACK);
                const bool showAttack = (battleMode_ != BattleMode::MOVE);
                renderer_.drawRangeOverlay(window,
                                           pos,
                                           std::max(1, selected.stats.movement),
                                           maxAttackRange,
                                           showMove,
                                           showAttack);
                break;
            }
        }
    }
    
    renderer_.drawUI(window, gameState_, myPlayerId_, uiInfoLines_);
}

void SFMLGameClient::handleMouseClick(sf::Vector2i mousePos, sf::RenderWindow& window) {
    (void)window;  // Suppress unused warning
    sf::Vector2f worldPos(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
    
    switch (gameState_.phase) {
        case GamePhase::SELECTION: {
            const auto clickedClass = renderer_.getSelectionClassAt(worldPos);
            const bool readyEnabled = selectedClasses_.size() == static_cast<std::size_t>(GameState::CHARS_PER_PLAYER);
            if (clickedClass.has_value()) {
                if (!selectionReadySent_) {
                    selectCharacter(clickedClass.value());
                    recentlySelectedClass_ = clickedClass;
                    selectionFlashUntilSeconds_ = selectionElapsedSeconds_ + 0.35f;
                }
                break;
            }

            if (renderer_.isSelectionReadyButtonAt(worldPos) && readyEnabled && !selectionReadySent_) {
                sendReady();
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
                
                if (currentPlacementIndex_ < static_cast<int>(myClasses.size())) {
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

void SFMLGameClient::handleMouseMove(sf::Vector2i mousePos) {
    if (gameState_.phase != GamePhase::SELECTION) {
        hoveredSelectionClass_.reset();
        readyButtonHovered_ = false;
        return;
    }
    const sf::Vector2f worldPos(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
    hoveredSelectionClass_ = renderer_.getSelectionClassAt(worldPos);
    const bool readyEnabled = selectedClasses_.size() == static_cast<std::size_t>(GameState::CHARS_PER_PLAYER);
    readyButtonHovered_ = readyEnabled && !selectionReadySent_ && renderer_.isSelectionReadyButtonAt(worldPos);
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
                pushUiInfo("Mode: MOVE (click destination hex)");
            }
            break;
            
        case sf::Keyboard::Key::A:
            if (gameState_.phase == GamePhase::PLAYING && !selectedCharacterId_.empty()) {
                battleMode_ = BattleMode::ATTACK;
                std::cout << "Mode: ATTACK - Click a hex to attack" << std::endl;
                pushUiInfo("Mode: ATTACK (click target hex)");
            }
            break;
            
        default:
            break;
    }
}

void SFMLGameClient::selectCharacter(CharacterClass charClass) {
    std::cout << "Selecting class: " << static_cast<int>(charClass) << std::endl;
    pushUiInfo("Toggled class: " + std::to_string(static_cast<int>(charClass)));
    sendMessage(Protocol::selectCharacterMessage(myPlayerId_, charClass));
}

void SFMLGameClient::sendReady() {
    selectionReadySent_ = true;
    std::cout << "Ready! Waiting for opponent..." << std::endl;
    pushUiInfo("Ready sent. Waiting for opponent...");
    sendMessage(Protocol::readyMessage(myPlayerId_));
}

void SFMLGameClient::placeCharacter(CharacterClass charClass, const HexCoord& pos) {
    std::cout << "Placing character class " << static_cast<int>(charClass) 
              << " at (" << pos.q << ", " << pos.r << ")" << std::endl;
    pushUiInfo("Placing class " + std::to_string(static_cast<int>(charClass)) + " at (" +
               std::to_string(pos.q) + "," + std::to_string(pos.r) + ")");
    sendMessage(Protocol::placeCharacterMessage(myPlayerId_, charClass, pos));
}

void SFMLGameClient::submitAction(const Action& action) {
    sendMessage(Protocol::submitActionMessage(action));
}

void SFMLGameClient::lockTurn() {
    std::cout << "Locking turn..." << std::endl;
    pushUiInfo("Turn locked.");
    
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
    pushUiInfo("Selected " + characterId + ". Press M(move) or A(attack).");
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
        pushUiInfo("This character already has a queued action.");
        return;
    }
    
    Action action;
    action.characterId = selectedCharacterId_;
    
    if (battleMode_ == BattleMode::MOVE) {
        action.type = ActionType::MOVE;
        action.target = targetHex;
        std::cout << "Queued MOVE for " << selectedCharacterId_ 
                  << " to (" << targetHex.q << ", " << targetHex.r << ")" << std::endl;
        pushUiInfo("Queued MOVE to (" + std::to_string(targetHex.q) + "," + std::to_string(targetHex.r) + ")");
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
                    pushUiInfo("Queued ATTACK at (" + std::to_string(targetHex.q) + "," + std::to_string(targetHex.r) + ")");
                    foundSkill = true;
                    break;
                }
            }
            
            if (!foundSkill) {
                std::cout << "No attack skill found for this character!" << std::endl;
                pushUiInfo("No attack skill found for selected character.");
                return;
            }
        }
    }
    
    pendingActions_.push_back(action);
    clearSelection();
}

void SFMLGameClient::pushUiInfo(const std::string& text) {
    if (text.empty()) {
        return;
    }
    uiInfoLines_.push_back(text);
    constexpr std::size_t kMaxInfoLines = 7;
    while (uiInfoLines_.size() > kMaxInfoLines) {
        uiInfoLines_.erase(uiInfoLines_.begin());
    }
}
