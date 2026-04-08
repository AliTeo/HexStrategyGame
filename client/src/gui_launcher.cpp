#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <string>
#include <cstdlib>

#ifdef _WIN32
#include <windows.h>
#endif

int main() {
    #ifdef _WIN32
    // Hide console window on Windows
    FreeConsole();
    #endif

    // Create window
    sf::RenderWindow window(sf::VideoMode(500, 350), "Hex Strategy - Connect to Game", sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(60);

    // Load font (use system font or bundled font)
    sf::Font font;
    #ifdef _WIN32
    if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
        return 1; // Fallback if no font found
    }
    #else
    if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
        return 1;
    }
    #endif

    // UI Elements
    sf::Text titleText("Connect to Hex Strategy Game", font, 24);
    titleText.setPosition(50, 30);
    titleText.setFillColor(sf::Color::White);

    sf::Text ipLabel("Server IP Address:", font, 16);
    ipLabel.setPosition(50, 100);
    ipLabel.setFillColor(sf::Color::White);

    sf::Text portLabel("Port:", font, 16);
    portLabel.setPosition(50, 170);
    portLabel.setFillColor(sf::Color::White);

    // Input boxes
    sf::RectangleShape ipBox(sf::Vector2f(400, 40));
    ipBox.setPosition(50, 130);
    ipBox.setFillColor(sf::Color(50, 50, 50));
    ipBox.setOutlineColor(sf::Color::White);
    ipBox.setOutlineThickness(2);

    sf::RectangleShape portBox(sf::Vector2f(400, 40));
    portBox.setPosition(50, 200);
    portBox.setFillColor(sf::Color(50, 50, 50));
    portBox.setOutlineColor(sf::Color::White);
    portBox.setOutlineThickness(2);

    // Input text
    std::string ipInput = "localhost";
    std::string portInput = "12345";
    bool ipActive = false;
    bool portActive = false;

    sf::Text ipText(ipInput, font, 18);
    ipText.setPosition(60, 138);
    ipText.setFillColor(sf::Color::White);

    sf::Text portText(portInput, font, 18);
    portText.setPosition(60, 208);
    portText.setFillColor(sf::Color::White);

    // Connect button
    sf::RectangleShape connectButton(sf::Vector2f(200, 50));
    connectButton.setPosition(150, 270);
    connectButton.setFillColor(sf::Color(0, 150, 0));

    sf::Text connectText("Connect", font, 20);
    connectText.setPosition(195, 280);
    connectText.setFillColor(sf::Color::White);

    // Main loop
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            // Mouse clicks
            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);

                // Check IP box click
                if (ipBox.getGlobalBounds().contains(mousePos)) {
                    ipActive = true;
                    portActive = false;
                    ipBox.setOutlineColor(sf::Color::Cyan);
                    portBox.setOutlineColor(sf::Color::White);
                }
                // Check port box click
                else if (portBox.getGlobalBounds().contains(mousePos)) {
                    portActive = true;
                    ipActive = false;
                    portBox.setOutlineColor(sf::Color::Cyan);
                    ipBox.setOutlineColor(sf::Color::White);
                }
                // Check connect button
                else if (connectButton.getGlobalBounds().contains(mousePos)) {
                    window.close();
                    
                    // Launch the actual game client
                    std::string command;
                    #ifdef _WIN32
                    command = "HexGameClient.exe " + ipInput + " " + portInput;
                    system(command.c_str());
                    #else
                    command = "./HexGameClient " + ipInput + " " + portInput;
                    system(command.c_str());
                    #endif
                    
                    return 0;
                }
                else {
                    ipActive = false;
                    portActive = false;
                    ipBox.setOutlineColor(sf::Color::White);
                    portBox.setOutlineColor(sf::Color::White);
                }
            }

            // Text input
            if (event.type == sf::Event::TextEntered) {
                if (ipActive) {
                    if (event.text.unicode == 8 && !ipInput.empty()) { // Backspace
                        ipInput.pop_back();
                    } else if (event.text.unicode < 128 && event.text.unicode != 8) {
                        ipInput += static_cast<char>(event.text.unicode);
                    }
                    ipText.setString(ipInput);
                }
                else if (portActive) {
                    if (event.text.unicode == 8 && !portInput.empty()) { // Backspace
                        portInput.pop_back();
                    } else if (event.text.unicode >= '0' && event.text.unicode <= '9') {
                        portInput += static_cast<char>(event.text.unicode);
                    }
                    portText.setString(portInput);
                }
            }

            // Enter key to connect
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
                window.close();
                
                std::string command;
                #ifdef _WIN32
                command = "HexGameClient.exe " + ipInput + " " + portInput;
                system(command.c_str());
                #else
                command = "./HexGameClient " + ipInput + " " + portInput;
                system(command.c_str());
                #endif
                
                return 0;
            }
        }

        // Draw everything
        window.clear(sf::Color(30, 30, 30));
        window.draw(titleText);
        window.draw(ipLabel);
        window.draw(portLabel);
        window.draw(ipBox);
        window.draw(portBox);
        window.draw(ipText);
        window.draw(portText);
        window.draw(connectButton);
        window.draw(connectText);
        window.display();
    }

    return 0;
}
