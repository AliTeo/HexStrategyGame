# SFML 3.0.2 Integration

This project now includes SFML (Simple and Fast Multimedia Library) version 3.0.2 for graphics, window management, and audio.

## Prerequisites

Before building with SFML support, you need to install:

### Arch Linux / Manjaro
```bash
sudo pacman -S cmake git base-devel libx11 libxrandr libxcursor freetype2 mesa openal flac libvorbis
```

### Ubuntu / Debian
```bash
sudo apt install cmake git build-essential libx11-dev libxrandr-dev libxcursor-dev libfreetype-dev libgl1-mesa-dev libudev-dev libopenal-dev libflac-dev libvorbis-dev
```

### Fedora / RHEL
```bash
sudo dnf install cmake git gcc-c++ libX11-devel libXrandr-devel libXcursor-devel freetype-devel mesa-libGL-devel systemd-devel openal-soft-devel flac-devel libvorbis-devel
```

## Setup Instructions

1. **Install dependencies** (see above for your distro)

2. **Download and build SFML 3.0.2:**
   ```bash
   ./setup_sfml.sh
   ```
   
   This will:
   - Clone SFML 3.0.2 from GitHub
   - Build it as static libraries
   - Install to `external/SFML/install/`

3. **Build the game:**
   ```bash
   ./build.sh
   ```

## What's Included

SFML modules included in this project:
- **SFML Graphics** - 2D graphics, sprites, textures, text rendering
- **SFML Window** - Window creation and event handling  
- **SFML System** - Core utilities (vectors, time, threads)
- **SFML Audio** - Sound and music playback

The Network module is excluded as we use Boost.Asio for networking.

## Using SFML in Your Code

### Basic Window Example (SFML 3.0 Syntax)

```cpp
#include <SFML/Graphics.hpp>

int main() {
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Hexagonal Strategy Game");
    
    while (window.isOpen()) {
        // SFML 3.0: pollEvent() returns std::optional<sf::Event>
        while (std::optional event = window.pollEvent()) {
            // Use is<T>() to check event type
            if (event->is<sf::Event::Closed>())
                window.close();
            
            // Handle keyboard input
            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->code == sf::Keyboard::Key::Escape)
                    window.close();
            }
        }
        
        window.clear(sf::Color::Black);
        // Draw your game here
        window.display();
    }
    
    return 0;
}
```

### Key Differences from SFML 2.x

SFML 3.0 has significant breaking changes from 2.x:

1. **Event handling (MAJOR CHANGE)**:
   ```cpp
   // SFML 2.x (OLD)
   sf::Event event;
   while (window.pollEvent(event)) {
       if (event.type == sf::Event::Closed)
           window.close();
   }
   
   // SFML 3.0 (NEW)
   while (std::optional event = window.pollEvent()) {
       if (event->is<sf::Event::Closed>())
           window.close();
   }
   ```

2. **VideoMode constructor**: 
   ```cpp
   // SFML 2.x: sf::VideoMode(800, 600)
   // SFML 3.0: sf::VideoMode({800, 600})
   ```

3. **Getting event data**:
   ```cpp
   // SFML 3.0: Use getIf<T>() to get event data
   if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
       std::cout << "Key: " << static_cast<int>(keyPressed->code) << std::endl;
   }
   ```

4. **Vector constructors**: 
   ```cpp
   // Use uniform initialization: sf::Vector2f{x, y}
   ```

5. **Improved const-correctness** and modern C++ features throughout

## File Locations

- **SFML source**: `external/SFML/`
- **SFML headers**: `external/SFML/install/include/`
- **SFML libraries**: `external/SFML/install/lib/`

## Troubleshooting

### CMake not found
```bash
sudo pacman -S cmake  # Arch
sudo apt install cmake # Ubuntu
```

### Missing system libraries
Make sure all dependencies listed above are installed for your distribution.

### Build errors
Clean and rebuild:
```bash
rm -rf external/SFML
./setup_sfml.sh
./build.sh
```

## Documentation

- [SFML 3.0 Documentation](https://www.sfml-dev.org/documentation/3.0.0/)
- [SFML Tutorials](https://www.sfml-dev.org/tutorials/3.0/)
- [SFML GitHub](https://github.com/SFML/SFML)
