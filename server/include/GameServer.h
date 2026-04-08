#pragma once

#include "MatchManager.h"
#include "Protocol.h"
#include <boost/asio.hpp>
#include <memory>
#include <set>
#include <thread>
#include <deque>

using boost::asio::ip::tcp;

/**
 * Client connection handler
 */
class ClientConnection : public std::enable_shared_from_this<ClientConnection> {
public:
    ClientConnection(tcp::socket socket, class GameServer& server);
    
    void start();
    void send(const std::string& message);
    void close();
    
    int getPlayerId() const { return playerId; }
    void setPlayerId(int id) { playerId = id; }
    
    tcp::socket socket_;  // Made public for server access
    
private:
    void readMessage();
    void handleMessage(const std::string& message);
    void write();
    
    GameServer& server_;
    boost::asio::streambuf buffer_;
    std::deque<std::string> writeQueue_;
    int playerId;
    bool isWriting_;
};

/**
 * Main game server
 */
class GameServer {
public:
    GameServer(unsigned short port);
    ~GameServer();
    
    void run();
    void stop();
    
    // Called by ClientConnection
    void handleClientMessage(std::shared_ptr<ClientConnection> client, 
                            MessageType type, const nlohmann::json& data);
    void clientDisconnected(std::shared_ptr<ClientConnection> client);
    
    // Get match manager
    MatchManager& getMatchManager() { return matchManager_; }
    
    // Broadcast to session
    void broadcastToSession(int sessionId, const std::string& message);
    void sendToPlayer(int playerId, const std::string& message);
    
private:
    void startAccept();
    void handleAccept(std::shared_ptr<ClientConnection> client, const boost::system::error_code& error);
    
    // Player ID assignment
    int assignPlayerId();
    
    boost::asio::io_context ioContext_;
    tcp::acceptor acceptor_;
    MatchManager matchManager_;
    
    std::set<std::shared_ptr<ClientConnection>> clients_;
    std::map<int, std::shared_ptr<ClientConnection>> playerConnections_;
    std::map<int, int> sessionPlayers_;  // playerId -> sessionId
    
    int nextPlayerId_;
    bool running_;
};
