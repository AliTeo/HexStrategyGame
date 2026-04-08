#include "GameServer.h"
#include <iostream>
#include <csignal>

GameServer* g_server = nullptr;

void signalHandler(int signal) {
    if (g_server) {
        std::cout << "\nShutting down server..." << std::endl;
        g_server->stop();
    }
}

int main(int argc, char* argv[]) {
    unsigned short port = 12345;
    
    if (argc > 1) {
        port = static_cast<unsigned short>(std::atoi(argv[1]));
    }
    
    try {
        std::cout << "========================================" << std::endl;
        std::cout << "  Hexagonal Strategy Game Server" << std::endl;
        std::cout << "========================================" << std::endl;
        std::cout << "Port: " << port << std::endl;
        std::cout << "Waiting for players..." << std::endl;
        std::cout << std::endl;
        
        GameServer server(port);
        g_server = &server;
        
        // Handle Ctrl+C gracefully
        std::signal(SIGINT, signalHandler);
        std::signal(SIGTERM, signalHandler);
        
        server.run();
        
    } catch (std::exception& e) {
        std::cerr << "Server error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
