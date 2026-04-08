#include <SFML/Graphics.hpp>
#include <iostream>

// Simple SFML test program to verify installation
int main() {
    std::cout << "Testing SFML 3.0.2..." << std::endl;
    
    // Create a window
    sf::RenderWindow window(sf::VideoMode({400, 300}), "SFML Test");
    window.setFramerateLimit(60);
    
    // Create a circle shape
    sf::CircleShape circle(50.0f);
    circle.setFillColor(sf::Color::Green);
    circle.setPosition({150.0f, 100.0f});
    
    std::cout << "Window created successfully!" << std::endl;
    std::cout << "Close the window to exit." << std::endl;
    
    // Main loop - SFML 3.0 event handling
    while (window.isOpen()) {
        // pollEvent() now returns std::optional<sf::Event>
        while (std::optional event = window.pollEvent()) {
            // Use is<T>() to check event type
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }
        
        window.clear(sf::Color::Black);
        window.draw(circle);
        window.display();
    }
    
    std::cout << "SFML test completed successfully!" << std::endl;
    return 0;
}
