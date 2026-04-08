#include "SFMLGameClient.h"
#include <SFML/Graphics.hpp>

#include <array>
#include <charconv>
#include <cctype>
#include <optional>
#include <string>
#include <vector>

namespace {

constexpr float kWindowWidth = 900.0f;
constexpr float kWindowHeight = 640.0f;
constexpr sf::Color kBackgroundColor(18, 22, 30);
constexpr sf::Color kPanelColor(35, 40, 52);
constexpr sf::Color kAccentColor(54, 136, 255);
constexpr sf::Color kTextColor(235, 238, 245);
constexpr sf::Color kErrorColor(255, 115, 115);
constexpr sf::Color kSuccessColor(132, 234, 136);

enum class Screen {
    MainMenu,
    JoinForm,
    Joining
};

struct JoinRequest {
    std::string host = "127.0.0.1";
    std::string port = "12345";
    std::string password;
    std::string username = "Player";
};

struct Button {
    sf::FloatRect bounds;
    std::string label;
    bool hovered = false;

    [[nodiscard]] bool contains(sf::Vector2f point) const {
        return bounds.contains(point);
    }
};

struct InputField {
    std::string label;
    std::string value;
    sf::FloatRect bounds;
    bool active = false;
    bool numericOnly = false;
    bool secret = false;

    [[nodiscard]] bool contains(sf::Vector2f point) const {
        return bounds.contains(point);
    }

    [[nodiscard]] std::string displayValue() const {
        if (!secret) {
            return value;
        }
        return std::string(value.size(), '*');
    }
};

std::optional<sf::Font> loadMenuFont() {
    const std::array<std::string, 4> candidates{
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
        "/usr/share/fonts/TTF/DejaVuSans.ttf",
        "/System/Library/Fonts/Supplemental/Arial Unicode.ttf"
    };

    sf::Font font;
    for (const auto& path : candidates) {
        if (font.openFromFile(path)) {
            return font;
        }
    }
    return std::nullopt;
}

void drawText(sf::RenderWindow& window,
              const sf::Font& font,
              const std::string& text,
              unsigned int size,
              sf::Vector2f position,
              sf::Color color = kTextColor) {
    sf::Text drawable(font, text, size);
    drawable.setPosition(position);
    drawable.setFillColor(color);
    window.draw(drawable);
}

void drawButton(sf::RenderWindow& window, const sf::Font& font, const Button& button) {
    sf::RectangleShape rect(button.bounds.size);
    rect.setPosition(button.bounds.position);
    rect.setFillColor(button.hovered ? sf::Color(68, 75, 94) : kPanelColor);
    rect.setOutlineColor(kAccentColor);
    rect.setOutlineThickness(2.0f);
    window.draw(rect);

    sf::Text label(font, button.label, 24);
    const sf::FloatRect textBounds = label.getLocalBounds();
    const float x = button.bounds.position.x + (button.bounds.size.x - textBounds.size.x) * 0.5f - textBounds.position.x;
    const float y = button.bounds.position.y + (button.bounds.size.y - textBounds.size.y) * 0.5f - textBounds.position.y;
    label.setPosition({x, y});
    label.setFillColor(kTextColor);
    window.draw(label);
}

void drawField(sf::RenderWindow& window, const sf::Font& font, const InputField& field) {
    drawText(window, font, field.label, 18, {field.bounds.position.x, field.bounds.position.y - 26.0f});

    sf::RectangleShape rect(field.bounds.size);
    rect.setPosition(field.bounds.position);
    rect.setFillColor(sf::Color(26, 31, 41));
    rect.setOutlineColor(field.active ? kAccentColor : sf::Color(97, 106, 130));
    rect.setOutlineThickness(2.0f);
    window.draw(rect);

    drawText(window, font, field.displayValue(), 22, {field.bounds.position.x + 12.0f, field.bounds.position.y + 10.0f});
}

bool parsePort(const std::string& text) {
    if (text.empty()) {
        return false;
    }
    int value = 0;
    const auto* begin = text.data();
    const auto* end = text.data() + text.size();
    const auto [ptr, ec] = std::from_chars(begin, end, value);
    return ec == std::errc{} && ptr == end && value > 0 && value <= 65535;
}

void setActiveField(std::vector<InputField>& fields, std::size_t index) {
    for (std::size_t i = 0; i < fields.size(); ++i) {
        fields[i].active = (i == index);
    }
}

}  // namespace

int main(int argc, char* argv[]) {
    auto fontOpt = loadMenuFont();
    if (!fontOpt.has_value()) {
        return 1;
    }
    const sf::Font& font = *fontOpt;

    sf::RenderWindow window(sf::VideoMode({static_cast<unsigned int>(kWindowWidth), static_cast<unsigned int>(kWindowHeight)}),
                            "Hexagonal Strategy Game");
    window.setFramerateLimit(60);

    Screen screen = Screen::MainMenu;
    JoinRequest request;
    bool startJoinNow = false;

    if (argc > 1) {
        request.host = argv[1];
        startJoinNow = true;
        screen = Screen::Joining;
    }
    if (argc > 2) {
        request.port = argv[2];
    }

    std::vector<InputField> fields{
        {"Server IP", request.host, {{170.0f, 190.0f}, {560.0f, 46.0f}}, false, false, false},
        {"Port", request.port, {{170.0f, 270.0f}, {560.0f, 46.0f}}, false, true, false},
        {"Password", "", {{170.0f, 350.0f}, {560.0f, 46.0f}}, false, false, true},
        {"Username", request.username, {{170.0f, 430.0f}, {560.0f, 46.0f}}, false, false, false},
    };

    Button hostButton{{{280.0f, 220.0f}, {340.0f, 68.0f}}, "Host"};
    Button joinButton{{{280.0f, 320.0f}, {340.0f, 68.0f}}, "Join"};
    Button exitButton{{{280.0f, 420.0f}, {340.0f, 68.0f}}, "Exit"};
    Button submitJoinButton{{{170.0f, 520.0f}, {270.0f, 60.0f}}, "Join"};
    Button backButton{{{460.0f, 520.0f}, {270.0f, 60.0f}}, "Back"};

    SFMLGameClient client;
    sf::Clock retryClock;
    sf::Time nextRetryAt = sf::Time::Zero;
    bool retryPending = true;
    std::string statusLine1;
    std::string statusLine2;
    sf::Color statusColor = kTextColor;

    const auto beginJoining = [&]() {
        request.host = fields[0].value;
        request.port = fields[1].value;
        request.password = fields[2].value;
        request.username = fields[3].value;
        screen = Screen::Joining;
        retryClock.restart();
        nextRetryAt = sf::Time::Zero;
        retryPending = true;
        statusLine1 = "Joining as " + request.username;
        statusLine2.clear();
        statusColor = kTextColor;
    };

    if (startJoinNow) {
        statusLine1 = "Joining as " + request.username;
        statusLine2.clear();
    }

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }

            if (const auto* mouseMoved = event->getIf<sf::Event::MouseMoved>()) {
                const sf::Vector2f mouse(static_cast<float>(mouseMoved->position.x), static_cast<float>(mouseMoved->position.y));
                if (screen == Screen::MainMenu) {
                    hostButton.hovered = hostButton.contains(mouse);
                    joinButton.hovered = joinButton.contains(mouse);
                    exitButton.hovered = exitButton.contains(mouse);
                } else if (screen == Screen::JoinForm) {
                    submitJoinButton.hovered = submitJoinButton.contains(mouse);
                    backButton.hovered = backButton.contains(mouse);
                }
            }

            if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mousePressed->button == sf::Mouse::Button::Left) {
                    const sf::Vector2f mouse(static_cast<float>(mousePressed->position.x), static_cast<float>(mousePressed->position.y));

                    if (screen == Screen::MainMenu) {
                        if (hostButton.contains(mouse)) {
                            fields[0].value = "127.0.0.1";
                            fields[1].value = "12345";
                            fields[2].value.clear();
                            fields[3].value = "Host";
                            beginJoining();
                        } else if (joinButton.contains(mouse)) {
                            screen = Screen::JoinForm;
                            setActiveField(fields, 0);
                        } else if (exitButton.contains(mouse)) {
                            window.close();
                        }
                    } else if (screen == Screen::JoinForm) {
                        bool clickedField = false;
                        for (std::size_t i = 0; i < fields.size(); ++i) {
                            if (fields[i].contains(mouse)) {
                                setActiveField(fields, i);
                                clickedField = true;
                                break;
                            }
                        }

                        if (!clickedField) {
                            if (submitJoinButton.contains(mouse)) {
                                if (fields[0].value.empty() || fields[1].value.empty() || fields[3].value.empty()) {
                                    statusLine1 = "Server IP, port, and username are required.";
                                    statusLine2.clear();
                                    statusColor = kErrorColor;
                                } else if (!parsePort(fields[1].value)) {
                                    statusLine1 = "Port must be a number between 1 and 65535.";
                                    statusLine2.clear();
                                    statusColor = kErrorColor;
                                } else {
                                    beginJoining();
                                }
                            } else if (backButton.contains(mouse)) {
                                screen = Screen::MainMenu;
                                for (auto& field : fields) {
                                    field.active = false;
                                }
                                statusLine1.clear();
                                statusLine2.clear();
                            } else {
                                for (auto& field : fields) {
                                    field.active = false;
                                }
                            }
                        }
                    }
                }
            }

            if (screen == Screen::JoinForm) {
                if (const auto* textEntered = event->getIf<sf::Event::TextEntered>()) {
                    if (textEntered->unicode >= 32 && textEntered->unicode < 127) {
                        const char c = static_cast<char>(textEntered->unicode);
                        for (auto& field : fields) {
                            if (!field.active) {
                                continue;
                            }
                            if (!field.numericOnly || std::isdigit(static_cast<unsigned char>(c))) {
                                if (field.value.size() < 64) {
                                    field.value.push_back(c);
                                }
                            }
                            break;
                        }
                    }
                }

                if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                    if (keyPressed->code == sf::Keyboard::Key::Backspace) {
                        for (auto& field : fields) {
                            if (field.active && !field.value.empty()) {
                                field.value.pop_back();
                                break;
                            }
                        }
                    } else if (keyPressed->code == sf::Keyboard::Key::Enter) {
                        if (!fields[0].value.empty() && !fields[1].value.empty() && !fields[3].value.empty() && parsePort(fields[1].value)) {
                            beginJoining();
                        } else {
                            statusLine1 = "Enter valid server IP, port, and username.";
                            statusLine2.clear();
                            statusColor = kErrorColor;
                        }
                    }
                }
            }
        }

        if (screen == Screen::Joining) {
            const sf::Time elapsed = retryClock.getElapsedTime();
            if (!client.isConnected()) {
                if (retryPending && elapsed >= nextRetryAt) {
                    if (client.connect(request.host, request.port)) {
                        statusLine2 = "Found server. Waiting for opponent.";
                        statusColor = kSuccessColor;
                        retryPending = false;
                    } else {
                        nextRetryAt = elapsed + sf::seconds(10.0f);
                        statusLine2 = "Unable to find server. Trying again every 10 seconds.";
                        statusColor = kErrorColor;
                    }
                } else if (retryPending && elapsed < nextRetryAt) {
                    const int retryIn = static_cast<int>((nextRetryAt - elapsed).asSeconds()) + 1;
                    statusLine2 = "Unable to find server. Retrying in " + std::to_string(retryIn) + "s.";
                    statusColor = kErrorColor;
                }
            } else {
                client.pumpNetwork();
                statusLine2 = client.getLobbyStatus();
                if (!client.hasGameStarted()) {
                    statusLine2 = "Found server. Waiting for opponent.";
                }
                statusColor = kSuccessColor;
                if (client.hasGameStarted()) {
                    window.close();
                    client.run();
                    return 0;
                }
            }
        }

        window.clear(kBackgroundColor);
        drawText(window, font, "Hexagonal Strategy Game", 42, {180.0f, 62.0f});

        if (screen == Screen::MainMenu) {
            drawButton(window, font, hostButton);
            drawButton(window, font, joinButton);
            drawButton(window, font, exitButton);
        } else if (screen == Screen::JoinForm) {
            drawText(window, font, "Join Server", 34, {340.0f, 120.0f});
            for (const auto& field : fields) {
                drawField(window, font, field);
            }
            drawButton(window, font, submitJoinButton);
            drawButton(window, font, backButton);
        } else if (screen == Screen::Joining) {
            drawText(window, font, "Joining Server", 34, {310.0f, 170.0f});
            drawText(window, font, statusLine1, 24, {170.0f, 290.0f}, kTextColor);
            drawText(window, font, statusLine2, 24, {170.0f, 340.0f}, statusColor);
            drawText(window, font, "Server: " + request.host + ":" + request.port, 20, {170.0f, 400.0f}, sf::Color(173, 184, 212));
        }

        if (!statusLine1.empty() && screen == Screen::JoinForm) {
            drawText(window, font, statusLine1, 18, {170.0f, 590.0f}, statusColor);
        }

        window.display();
    }

    return 0;
}
