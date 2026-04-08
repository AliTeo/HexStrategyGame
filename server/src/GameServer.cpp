#include "GameServer.h"
#include <iostream>

// ClientConnection implementation
ClientConnection::ClientConnection(tcp::socket socket, GameServer& server)
    : socket_(std::move(socket)), server_(server), playerId(-1), isWriting_(false) {}

void ClientConnection::start() {
    readMessage();
}

void ClientConnection::send(const std::string& message) {
    auto self = shared_from_this();
    boost::asio::post(socket_.get_executor(), [this, self, message]() {
        writeQueue_.push_back(Protocol::addDelimiter(message));
        
        if (!isWriting_) {
            write();
        }
    });
}

void ClientConnection::close() {
    boost::system::error_code ec;
    socket_.close(ec);
}

void ClientConnection::readMessage() {
    auto self = shared_from_this();
    boost::asio::async_read_until(socket_, buffer_, Protocol::MESSAGE_DELIMITER,
        [this, self](const boost::system::error_code& error, std::size_t bytes_transferred) {
            if (!error) {
                std::string data{
                    boost::asio::buffers_begin(buffer_.data()),
                    boost::asio::buffers_begin(buffer_.data()) + bytes_transferred
                };
                buffer_.consume(bytes_transferred);
                
                std::string bufferStr = data;
                auto messages = Protocol::extractMessages(bufferStr);
                
                for (const auto& message : messages) {
                    handleMessage(message);
                }
                
                readMessage();
            } else {
                server_.clientDisconnected(self);
            }
        });
}

void ClientConnection::handleMessage(const std::string& message) {
    MessageType type;
    nlohmann::json data;
    
    if (Protocol::parseMessage(message, type, data)) {
        server_.handleClientMessage(shared_from_this(), type, data);
    }
}

void ClientConnection::write() {
    if (writeQueue_.empty()) {
        isWriting_ = false;
        return;
    }
    
    isWriting_ = true;
    auto self = shared_from_this();
    
    // Take the next message from the queue
    std::string messageToSend = writeQueue_.front();
    
    boost::asio::async_write(socket_, boost::asio::buffer(messageToSend),
        [this, self](const boost::system::error_code& error, std::size_t /*bytes_transferred*/) {
            if (!error) {
                writeQueue_.pop_front();
                
                // Continue writing if there are more messages
                if (!writeQueue_.empty()) {
                    write();
                } else {
                    isWriting_ = false;
                }
            } else {
                server_.clientDisconnected(self);
            }
        });
}

// GameServer implementation
GameServer::GameServer(unsigned short port)
    : ioContext_(), 
      acceptor_(ioContext_, tcp::endpoint(tcp::v4(), port)),
      nextPlayerId_(0),
      running_(false) {
    std::cout << "Server started on port " << port << std::endl;
}

GameServer::~GameServer() {
    stop();
}

void GameServer::run() {
    running_ = true;
    startAccept();
    ioContext_.run();
}

void GameServer::stop() {
    running_ = false;
    ioContext_.stop();
}

void GameServer::startAccept() {
    auto socket = std::make_shared<tcp::socket>(ioContext_);
    auto client = std::make_shared<ClientConnection>(std::move(*socket), *this);
    
    acceptor_.async_accept(client->socket_,
        [this, client](const boost::system::error_code& error) {
            handleAccept(client, error);
        });
}

void GameServer::handleAccept(std::shared_ptr<ClientConnection> client, const boost::system::error_code& error) {
    if (!error) {
        std::cout << "Client connected" << std::endl;
        
        // Assign player ID
        int playerId = assignPlayerId();
        client->setPlayerId(playerId);
        
        clients_.insert(client);
        playerConnections_[playerId] = client;
        
        // Start reading before sending any messages
        client->start();
        
        // Send connection confirmation
        client->send(Protocol::connectedMessage(playerId));
        
        // Assign to session
        auto session = matchManager_.assignPlayerToSession(playerId);
        if (session) {
            sessionPlayers_[playerId] = session->getSessionId();
            
            std::cout << "Player " << playerId << " assigned to session " << session->getSessionId() << std::endl;
            std::cout << "Session now has " << session->getPlayerCount() << " players" << std::endl;
            
            // Notify other players
            std::cout << "Broadcasting PLAYER_JOINED for player " << playerId << std::endl;
            broadcastToSession(session->getSessionId(), 
                Protocol::playerJoinedMessage(playerId));
            
            // If session is full, start the game
            if (session->isFull()) {
                std::cout << "Session " << session->getSessionId() << " is full, starting game" << std::endl;
                std::cout << "Broadcasting GAME_STARTED" << std::endl;
                broadcastToSession(session->getSessionId(), 
                    Protocol::gameStartedMessage());
                std::cout << "Broadcasting GAME_STATE_UPDATE" << std::endl;
                broadcastToSession(session->getSessionId(),
                    Protocol::gameStateUpdateMessage(session->getGameState()));
                std::cout << "Game started successfully" << std::endl;
            }
        }
    }
    
    if (running_) {
        startAccept();
    }
}

int GameServer::assignPlayerId() {
    return nextPlayerId_++;
}

void GameServer::handleClientMessage(std::shared_ptr<ClientConnection> client, 
                                     MessageType type, const nlohmann::json& data) {
    int playerId = client->getPlayerId();
    auto session = matchManager_.getPlayerSession(playerId);
    
    if (!session) {
        client->send(Protocol::errorMessage("Not in a game session"));
        return;
    }
    
    bool success = false;
    std::string errorMsg;
    
    switch (type) {
        case MessageType::SELECT_CHARACTER: {
            CharacterClass charClass = static_cast<CharacterClass>(data["class"].get<int>());
            success = session->handleSelectCharacter(playerId, charClass);
            if (success) {
                broadcastToSession(session->getSessionId(),
                    Protocol::gameStateUpdateMessage(session->getGameState()));
            } else {
                errorMsg = "Failed to select character";
            }
            break;
        }
        
        case MessageType::PLACE_CHARACTER: {
            CharacterClass charClass = static_cast<CharacterClass>(data["class"].get<int>());
            HexCoord pos = HexCoord::fromJson(data["position"]);
            success = session->handlePlaceCharacter(playerId, charClass, pos);
            if (success) {
                broadcastToSession(session->getSessionId(),
                    Protocol::gameStateUpdateMessage(session->getGameState()));
            } else {
                errorMsg = "Invalid placement";
            }
            break;
        }
        
        case MessageType::SUBMIT_ACTION: {
            Action action = Action::fromJson(data);
            success = session->handleAction(action);
            if (!success) {
                errorMsg = "Invalid action";
            }
            break;
        }
        
        case MessageType::LOCK_TURN: {
            success = session->handleLockTurn(playerId);
            if (success && session->shouldResolveTurn()) {
                // Resolve turn
                std::string summary = session->resolveTurn();
                broadcastToSession(session->getSessionId(),
                    Protocol::turnResolvedMessage(summary));
                broadcastToSession(session->getSessionId(),
                    Protocol::gameStateUpdateMessage(session->getGameState()));
            }
            break;
        }
        
        default:
            errorMsg = "Unknown message type";
            break;
    }
    
    if (!success && !errorMsg.empty()) {
        client->send(Protocol::errorMessage(errorMsg));
    }
}

void GameServer::clientDisconnected(std::shared_ptr<ClientConnection> client) {
    int playerId = client->getPlayerId();
    std::cout << "Client " << playerId << " disconnected" << std::endl;
    
    clients_.erase(client);
    playerConnections_.erase(playerId);
    
    // Remove from session
    if (sessionPlayers_.count(playerId)) {
        int sessionId = sessionPlayers_[playerId];
        sessionPlayers_.erase(playerId);
        
        // Notify other players in session
        broadcastToSession(sessionId, 
            Protocol::errorMessage("Player " + std::to_string(playerId) + " disconnected"));
    }
    
    matchManager_.removePlayerFromSessions(playerId);
}

void GameServer::broadcastToSession(int sessionId, const std::string& message) {
    std::cout << "Broadcasting to session " << sessionId << std::endl;
    int count = 0;
    for (const auto& [playerId, pSessionId] : sessionPlayers_) {
        if (pSessionId == sessionId) {
            std::cout << "  Sending to player " << playerId << std::endl;
            sendToPlayer(playerId, message);
            count++;
        }
    }
    std::cout << "  Broadcast sent to " << count << " players" << std::endl;
}

void GameServer::sendToPlayer(int playerId, const std::string& message) {
    auto it = playerConnections_.find(playerId);
    if (it != playerConnections_.end()) {
        it->second->send(message);
    }
}
