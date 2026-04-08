#include "GameClient.h"
#include <iostream>

int main(int argc, char* argv[]) {
    std::string host = "localhost";
    std::string port = "12345";
    
    if (argc > 1) {
        host = argv[1];
    }
    if (argc > 2) {
        port = argv[2];
    }
    
    try {
        GameClient client;
        
        std::cout << "========================================" << std::endl;
        std::cout << "  Hexagonal Strategy Game Client" << std::endl;
        std::cout << "========================================" << std::endl;
        std::cout << "Connecting to " << host << ":" << port << "..." << std::endl;
        
        if (client.connect(host, port)) {
            client.run();
        } else {
            std::cerr << "Failed to connect to server." << std::endl;
            std::cerr << "Make sure the server is running at " << host << ":" << port << std::endl;
            return 1;
        }
        
    } catch (std::exception& e) {
        std::cerr << "Client error: " << e.what() << std::endl;
        return 1;
    }
    
    std::cout << "\nGoodbye!" << std::endl;
    return 0;
}
