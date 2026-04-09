#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <boost/asio.hpp>
#include <memory>
#include <string>
#include <thread>
#include <deque>
#include <mutex>
#include <atomic>
#include <optional>
#include <vector>

#include "GameState.h"
#include "Protocol.h"
#include "HexRenderer.h"

using boost::asio::ip::tcp;

class SFMLGameClient {
public:
    SFMLGameClient();
    ~SFMLGameClient();
    
    bool connect(const std::string& host, const std::string& port);
    void run();
    void pumpNetwork();
    bool isConnected() const noexcept;
    bool hasGameStarted() const noexcept;
    std::string getLobbyStatus() const;
    
private:
    // Network functions
    void disconnect();
    void startRead();
    void sendMessage(const std::string& message);
    void processMessages();
    void handleMessage(const std::string& message);
    void updateGameState(const GameState& newState);
    void setLobbyStatus(std::string status);
    
    // Game logic
    void handleSelectionPhase(sf::RenderTarget& window);
    void handlePlacementPhase(sf::RenderTarget& window);
    void handlePlayingPhase(sf::RenderTarget& window);
    
    // Input handling
    void handleMouseClick(sf::Vector2i mousePos, sf::RenderWindow& window);
    void handleMouseMove(sf::Vector2i mousePos);
    void handleKeyPress(sf::Keyboard::Key key);
    
    // Commands
    void selectCharacter(CharacterClass charClass);
    void placeCharacter(CharacterClass charClass, const HexCoord& pos);
    void submitAction(const Action& action);
    void lockTurn();
    void sendReady();
    void pushUiInfo(const std::string& text);
    
    // Network state
    boost::asio::io_context ioContext_;
    std::unique_ptr<tcp::socket> socket_;
    boost::asio::streambuf readBuffer_;
    std::string messageBuffer_;
    std::thread ioThread_;
    std::atomic_bool connected_;
    std::atomic_bool running_;
    std::atomic_bool gameStarted_;
    
    // Game state
    GameState gameState_;
    int myPlayerId_;
    int currentPlacementIndex_;  // Track which character we're placing (0, 1, or 2)
    mutable std::mutex messageMutex_;
    mutable std::mutex statusMutex_;
    std::string lobbyStatus_;
    
    // Rendering
    HexRenderer renderer_;
    
    // Selection state
    std::vector<CharacterClass> selectedClasses_;
    std::optional<CharacterClass> hoveredSelectionClass_;
    std::optional<CharacterClass> recentlySelectedClass_;
    float selectionElapsedSeconds_;
    float selectionFlashUntilSeconds_;
    bool selectionReadySent_;
    bool readyButtonHovered_;
    
    // Battle phase state
    enum class BattleMode {
        NONE,
        MOVE,
        ATTACK
    };
    
    std::string selectedCharacterId_;
    BattleMode battleMode_;
    std::vector<Action> pendingActions_;  // Actions queued for this turn
    std::vector<std::string> uiInfoLines_;

    sf::RenderTexture sceneTexture_;
    sf::Shader postShader_;
    bool postShaderLoaded_;
    
    // Helper functions for battle phase
    void clearSelection();
    void selectCharacterForCommand(const std::string& characterId);
    void executeCommand(const HexCoord& targetHex);
    bool hasActionForCharacter(const std::string& characterId) const;
    Character* findCharacterAtHex(const HexCoord& hex);
};
