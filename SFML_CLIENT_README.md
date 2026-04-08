# SFML Graphical Client

A graphical prototype client for the Hexagonal Strategy Game using SFML 3.0.2.

## Features

✅ **Visual Hex Grid** - Hexagonal battlefield rendered with SFML
✅ **Character Selection** - Click to select character classes
✅ **Character Placement** - Click hexes to place characters
✅ **Real-time Networking** - Connects to game server via Boost.Asio
✅ **Color-coded Teams** - Blue (Player 0) vs Red (Player 1)
✅ **Health Bars** - Visual HP indicators for each character
✅ **Phase Indicators** - Visual UI showing game phase

## Building

```bash
# Make sure SFML is set up first
./setup_sfml.sh  # Only needed once

# Build the SFML client
./build_sfml.sh
```

## Running

1. **Start the server:**
   ```bash
   ./bin/HexGameServer
   ```

2. **Start SFML clients (2 terminals):**
   ```bash
   ./bin/HexGameSFML
   ```

## Controls

### Selection Phase
- **Mouse Click** on character boxes to select 3 characters

### Placement Phase
- **Mouse Click** on hexes to place your selected characters
- Place on LEFT side (negative q) if Player 0
- Place on RIGHT side (positive q) if Player 1

### Battle Phase
1. **Click on your character** to select it (yellow highlight appears)
2. Press **M** for Move mode OR **A** for Attack mode
3. **Click target hex** to queue action (blue arrow = move, red arrow = attack)
4. Repeat for other characters (unassigned characters auto-PASS)
5. Press **Space** to submit all actions and lock your turn
6. Press **Escape** to quit

📖 **Detailed guide:** See [SFML_BATTLE_CONTROLS.md](SFML_BATTLE_CONTROLS.md)

## Visual Legend

### Team Colors
- **Blue outline** - Player 0's characters
- **Red outline** - Player 1's characters

### Character Classes
- **Red** - Warrior (W)
- **Purple** - Mage (M)
- **Yellow** - Healer (H)
- **Gray** - Rogue (R)
- **Brown** - Tank (T)
- **Green** - Summoner (S)

### Grid Colors
- **Dark blue tint** - Player 0 territory (left, q < 0)
- **Dark red tint** - Player 1 territory (right, q > 0)
- **Neutral gray** - Middle column (q = 0)

## Comparison with Text Client

| Feature | Text Client | SFML Client |
|---------|-------------|-------------|
| Interface | Terminal commands | Point-and-click |
| Grid View | ASCII art | Visual hexagons |
| Character Info | Text stats | Color-coded circles + health bars |
| Input | Type commands | Mouse + keyboard |
| Visibility | All info visible | Clean visual UI |

## Architecture

```
SFMLGameClient (main game loop)
    ├── HexRenderer (visual rendering)
    │   ├── Hex grid drawing
    │   ├── Character rendering
    │   ├── UI panels
    │   └── Phase-specific screens
    └── Network layer (Boost.Asio)
        ├── Async message handling
        ├── Protocol encoding/decoding
        └── Game state synchronization
```

## Future Enhancements

Potential additions:
- [ ] Text labels (requires font file)
- [ ] Character animations
- [ ] Skill effect visualizations
- [ ] Mouse-over tooltips
- [ ] Turn indicators
- [ ] Victory/defeat animations
- [ ] Sound effects
- [ ] Background music
- [ ] Minimap
- [ ] Chat system

## Files

- `client/include/SFMLGameClient.h` - Main SFML client
- `client/include/HexRenderer.h` - Rendering system
- `client/src/SFMLGameClient.cpp` - Client implementation
- `client/src/HexRenderer.cpp` - Rendering implementation
- `sfml_main.cpp` - Entry point
- `build_sfml.sh` - Build script

## Notes

- SFML 3.0 uses modern C++ APIs (std::optional, sf::Vector2f, sf::Angle)
- The renderer uses a flat-top hexagon layout
- All networking is handled asynchronously in a background thread
- Rendering runs at 60 FPS

Enjoy the game! 🎮
