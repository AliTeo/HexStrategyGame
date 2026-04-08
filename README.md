# Hexagonal Strategy Game

A 2-player online multiplayer turn-based strategy game with hexagonal grid.

## Features

- **Hexagonal Grid**: 7x7 battlefield using axial coordinates
- **Character Selection**: Choose 3 characters from a pool of 6 unique classes
- **Simultaneous Turns**: Both players submit actions secretly, then resolve together
- **Skill System**: Unique abilities with combination mechanics
- **Authoritative Server**: Client-server architecture for fair gameplay

## Character Classes

1. **Warrior** - Melee specialist with area push abilities
2. **Mage** - Ranged damage dealer with terrain alteration
3. **Healer** - Support class with healing and shields
4. **Rogue** - High mobility with backstab mechanics
5. **Tank** - Defensive powerhouse with crowd control
6. **Summoner** - Summons units and controls hexes

## Building

### Dependencies

- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- CMake 3.15+
- ASIO (header-only, included in external/)
- nlohmann/json (header-only, included in external/)

### Build Instructions

```bash
# Download dependencies first (see external/README.md)
cd HexGame
mkdir build && cd build
cmake ..
make

# Run server
./HexGameServer

# Run client (in another terminal)
./HexGameClient
```

## Gameplay

### Setup Phase
1. Connect to server
2. Wait for opponent
3. Select 3 characters from pool
4. Place characters on your side of the hex grid

### Turn Phase
1. View current game state
2. Submit actions for each character (move, skill, pass)
3. Lock in your turn
4. Wait for opponent to lock in
5. Watch simultaneous resolution

### Commands (Text Prototype)

- `select <character_id>` - Select character during setup
- `place <char> <q> <r>` - Place character at hex coordinates
- `move <char> <q> <r>` - Move character
- `skill <char> <skill_id> <target_q> <target_r>` - Use skill
- `pass <char>` - Skip character's action
- `ready` - Lock in your turn
- `view` - Display current game state
- `help` - Show available commands

## Technical Stack

- **Language**: C++17
- **Networking**: ASIO (standalone)
- **Serialization**: nlohmann/json
- **Future Graphics**: SFML 2.6+
- **Build System**: CMake

## Architecture

```
Client <--TCP/JSON--> Server <--TCP/JSON--> Client
   |                     |
   +-- Game State -------+-- Game Session
   +-- Input Handler         +-- Turn Resolver
   +-- Text Renderer         +-- Match Manager
```

## Project Status

**Current Phase**: Text-based prototype
**Next Phase**: SFML graphics integration

## License

MIT License - See LICENSE file for details
