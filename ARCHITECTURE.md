# Architecture Overview

## System Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                         GAME SYSTEM                              │
├─────────────────────────────────────────────────────────────────┤
│                                                                  │
│  ┌──────────────┐         ┌──────────────┐                     │
│  │   Client 1   │◄───────►│              │                     │
│  │  (Player 0)  │   TCP   │              │                     │
│  └──────────────┘         │              │                     │
│                           │              │                     │
│                           │    Server    │                     │
│                           │ (Port 12345) │                     │
│  ┌──────────────┐         │              │                     │
│  │   Client 2   │◄───────►│              │                     │
│  │  (Player 1)  │   TCP   │              │                     │
│  └──────────────┘         └──────────────┘                     │
│                                                                  │
└─────────────────────────────────────────────────────────────────┘
```

## Component Hierarchy

```
┌─────────────────────────────────────────────────────────────────┐
│                         GAME SERVER                              │
├─────────────────────────────────────────────────────────────────┤
│                                                                  │
│  ┌────────────────────────────────────────────────────┐         │
│  │              MatchManager                          │         │
│  │  - Creates/manages game sessions                   │         │
│  │  - Handles matchmaking                             │         │
│  └───────────────────┬────────────────────────────────┘         │
│                      │                                           │
│                      ▼                                           │
│  ┌────────────────────────────────────────────────────┐         │
│  │              GameSession                           │         │
│  │  - Manages one 2-player game                       │         │
│  │  - Contains GameState                              │         │
│  │  - Uses TurnResolver                               │         │
│  └───────────────────┬────────────────────────────────┘         │
│                      │                                           │
│         ┌────────────┴───────────┐                              │
│         ▼                        ▼                              │
│  ┌─────────────┐          ┌─────────────┐                      │
│  │  GameState  │          │TurnResolver │                      │
│  │  - Grid     │          │  - Executes │                      │
│  │  - Chars    │          │    actions  │                      │
│  │  - Phase    │          │  - Resolves │                      │
│  └─────────────┘          │    combat   │                      │
│                           └─────────────┘                      │
└─────────────────────────────────────────────────────────────────┘
```

## Client Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                         GAME CLIENT                              │
├─────────────────────────────────────────────────────────────────┤
│                                                                  │
│  ┌────────────────────────────────────────────────────┐         │
│  │              GameClient                            │         │
│  │  - Manages connection to server                    │         │
│  │  - Handles network I/O                             │         │
│  │  - Maintains local game state copy                 │         │
│  └───────┬─────────────┬──────────────┬───────────────┘         │
│          │             │              │                         │
│          ▼             ▼              ▼                         │
│  ┌──────────┐  ┌──────────┐  ┌──────────────┐                 │
│  │  Text    │  │  Input   │  │  MenuSystem  │                 │
│  │ Renderer │  │ Handler  │  │ (Placeholder)│                 │
│  │          │  │          │  │              │                 │
│  │ - Grid   │  │ - Parse  │  │ - Main Menu  │                 │
│  │ - Stats  │  │ - Cmds   │  │ - Pause      │                 │
│  └──────────┘  └──────────┘  └──────────────┘                 │
│                                                                  │
└─────────────────────────────────────────────────────────────────┘
```

## Common Library (Shared)

```
┌─────────────────────────────────────────────────────────────────┐
│                      COMMON LIBRARY                              │
├─────────────────────────────────────────────────────────────────┤
│                                                                  │
│  ┌──────────────┐      ┌──────────────┐      ┌──────────────┐  │
│  │  HexCoord    │      │  Character   │      │    Action    │  │
│  │              │      │              │      │              │  │
│  │ - q, r       │      │ - Stats      │      │ - Type       │  │
│  │ - Distance   │      │ - Skills     │      │ - Target     │  │
│  │ - Neighbors  │      │ - Factory    │      │ - Skill ID   │  │
│  └──────────────┘      └──────────────┘      └──────────────┘  │
│                                                                  │
│  ┌──────────────┐      ┌──────────────┐      ┌──────────────┐  │
│  │  GameState   │      │   Protocol   │      │    Skill     │  │
│  │              │      │              │      │              │  │
│  │ - Phase      │      │ - Messages   │      │ - Type       │  │
│  │ - Grid       │      │ - JSON       │      │ - Range      │  │
│  │ - Characters │      │ - Serialize  │      │ - Power      │  │
│  └──────────────┘      └──────────────┘      └──────────────┘  │
│                                                                  │
└─────────────────────────────────────────────────────────────────┘
```

## Game Flow State Machine

```
┌─────────────────────────────────────────────────────────────────┐
│                         GAME PHASES                              │
├─────────────────────────────────────────────────────────────────┤
│                                                                  │
│   ┌─────────┐                                                    │
│   │ WAITING │  (Server waiting for 2 players)                   │
│   └────┬────┘                                                    │
│        │ 2 players connected                                     │
│        ▼                                                         │
│   ┌──────────┐                                                   │
│   │SELECTION │  (Each player selects 3 of 6 characters)         │
│   └────┬─────┘                                                   │
│        │ Both selected 3                                         │
│        ▼                                                         │
│   ┌──────────┐                                                   │
│   │PLACEMENT │  (Place 3 characters on grid)                    │
│   └────┬─────┘                                                   │
│        │ Both placed all                                         │
│        ▼                                                         │
│   ┌──────────┐                                                   │
│   │ PLAYING  │  (Submit actions each turn)                      │
│   └────┬─────┘                                                   │
│        │ Both locked in                                          │
│        ▼                                                         │
│   ┌─────────────┐                                                │
│   │TURN_RESOLVE │  (Execute simultaneous actions)               │
│   └─────┬───────┘                                                │
│         │ Victory check                                          │
│         ├─► Not over ─► Back to PLAYING                          │
│         │                                                        │
│         └─► Victory! ─► GAME_OVER                                │
│                                                                  │
└─────────────────────────────────────────────────────────────────┘
```

## Network Protocol

```
┌─────────────────────────────────────────────────────────────────┐
│                      MESSAGE FLOW                                │
├─────────────────────────────────────────────────────────────────┤
│                                                                  │
│  Client                    Server                    Client     │
│    │                         │                         │        │
│    ├──CONNECT──────────────►│                         │        │
│    │◄──CONNECTED─────────────┤                         │        │
│    │                         │◄──CONNECT───────────────┤        │
│    │                         ├──CONNECTED─────────────►│        │
│    │                         ├──PLAYER_JOINED─────────►│        │
│    │◄──PLAYER_JOINED─────────┤                         │        │
│    │                         │                         │        │
│    ├──SELECT_CHARACTER──────►│                         │        │
│    │                         ├──SELECT_CHARACTER──────►│        │
│    │◄──GAME_STATE_UPDATE─────┤                         │        │
│    │                         ├──GAME_STATE_UPDATE─────►│        │
│    │                         │                         │        │
│    │     [Selection Phase Complete]                    │        │
│    │                         │                         │        │
│    ├──PLACE_CHARACTER───────►│                         │        │
│    │                         ├──PLACE_CHARACTER───────►│        │
│    │◄──GAME_STATE_UPDATE─────┤                         │        │
│    │                         ├──GAME_STATE_UPDATE─────►│        │
│    │                         │                         │        │
│    │     [Placement Phase Complete - Game Starts]      │        │
│    │                         │                         │        │
│    ├──SUBMIT_ACTION─────────►│                         │        │
│    ├──SUBMIT_ACTION─────────►│                         │        │
│    ├──LOCK_TURN─────────────►│                         │        │
│    │                         │◄──SUBMIT_ACTION─────────┤        │
│    │                         │◄──SUBMIT_ACTION─────────┤        │
│    │                         │◄──LOCK_TURN─────────────┤        │
│    │                         │                         │        │
│    │        [Turn Resolution]                          │        │
│    │                         │                         │        │
│    │◄──TURN_RESOLVED─────────┼──TURN_RESOLVED─────────►│        │
│    │◄──GAME_STATE_UPDATE─────┼──GAME_STATE_UPDATE─────►│        │
│    │                         │                         │        │
│    │     [Next Turn Begins - Repeat]                   │        │
│                                                                  │
└─────────────────────────────────────────────────────────────────┘
```

## Hexagonal Grid Layout

```
┌─────────────────────────────────────────────────────────────────┐
│                    7x7 HEX GRID (Radius 3)                       │
├─────────────────────────────────────────────────────────────────┤
│                                                                  │
│         r=-3:      (-3,-3)(-2,-3)(-1,-3)(0,-3)                  │
│         r=-2:    (-3,-2)(-2,-2)(-1,-2)(0,-2)(1,-2)              │
│         r=-1:  (-3,-1)(-2,-1)(-1,-1)(0,-1)(1,-1)(2,-1)          │
│         r= 0: (-3,0)(-2,0)(-1,0)(0,0)(1,0)(2,0)(3,0)            │
│         r= 1:   (-2,1)(-1,1)(0,1)(1,1)(2,1)(3,1)                │
│         r= 2:     (-1,2)(0,2)(1,2)(2,2)(3,2)                    │
│         r= 3:       (0,3)(1,3)(2,3)(3,3)                        │
│                                                                  │
│  Player 0 Zone (q < 0)  │  Neutral  │  Player 1 Zone (q > 0)    │
│        LEFT SIDE        │  (q = 0)  │       RIGHT SIDE          │
│                                                                  │
└─────────────────────────────────────────────────────────────────┘
```

## Data Flow Diagram

```
┌─────────────────────────────────────────────────────────────────┐
│                        DATA FLOW                                 │
├─────────────────────────────────────────────────────────────────┤
│                                                                  │
│  User Input                                                      │
│      │                                                           │
│      ▼                                                           │
│  InputHandler ──► Parse Command                                 │
│      │                                                           │
│      ▼                                                           │
│  GameClient ───► Create Protocol Message                        │
│      │                                                           │
│      ▼                                                           │
│  TCP Socket ───► Send to Server                                 │
│      │                                                           │
│      ▼                                                           │
│  GameServer ───► Validate Action                                │
│      │                                                           │
│      ▼                                                           │
│  GameSession ──► Update GameState                               │
│      │                                                           │
│      ▼                                                           │
│  TurnResolver ─► Execute Actions                                │
│      │                                                           │
│      ▼                                                           │
│  GameState ────► Generate Result                                │
│      │                                                           │
│      ▼                                                           │
│  Protocol ─────► Serialize to JSON                              │
│      │                                                           │
│      ▼                                                           │
│  TCP Socket ───► Broadcast to Clients                           │
│      │                                                           │
│      ▼                                                           │
│  GameClient ───► Update Local State                             │
│      │                                                           │
│      ▼                                                           │
│  TextRenderer ─► Display to User                                │
│                                                                  │
└─────────────────────────────────────────────────────────────────┘
```

## Directory Tree

```
HexGame/
├── bin/                          # Executables
│   ├── HexGameServer
│   └── HexGameClient
├── build/                        # Build artifacts
│   ├── common/
│   ├── server/
│   └── client/
├── common/                       # Shared code
│   ├── include/
│   │   ├── HexCoord.h
│   │   ├── Character.h
│   │   ├── Skill.h
│   │   ├── Action.h
│   │   ├── GameState.h
│   │   └── Protocol.h
│   └── src/
│       ├── HexCoord.cpp
│       ├── Character.cpp
│       ├── Skill.cpp
│       ├── Action.cpp
│       ├── GameState.cpp
│       └── Protocol.cpp
├── server/                       # Server code
│   ├── include/
│   │   ├── GameServer.h
│   │   ├── GameSession.h
│   │   ├── MatchManager.h
│   │   └── TurnResolver.h
│   └── src/
│       ├── GameServer.cpp
│       ├── GameSession.cpp
│       ├── MatchManager.cpp
│       ├── TurnResolver.cpp
│       └── main.cpp
├── client/                       # Client code
│   ├── include/
│   │   ├── GameClient.h
│   │   ├── TextRenderer.h
│   │   ├── InputHandler.h
│   │   └── MenuSystem.h
│   └── src/
│       ├── GameClient.cpp
│       ├── TextRenderer.cpp
│       ├── InputHandler.cpp
│       ├── MenuSystem.cpp
│       └── main.cpp
├── external/                     # Dependencies
│   ├── asio/
│   │   └── include/
│   └── json/
│       └── include/
│           └── nlohmann/
│               └── json.hpp
├── tests/                        # Future tests
├── build.sh                      # Build script
├── test_game.sh                  # Test script
├── CMakeLists.txt               # CMake config
├── README.md                    # Overview
├── TESTING.md                   # Test guide
├── PROJECT_SUMMARY.md           # Summary
└── ARCHITECTURE.md              # This file
```

## Class Relationships

```
┌─────────────────────────────────────────────────────────────────┐
│                     CLASS DIAGRAM                                │
├─────────────────────────────────────────────────────────────────┤
│                                                                  │
│  GameState                                                       │
│  ┌──────────────────────────────────────┐                       │
│  │ - phase: GamePhase                   │                       │
│  │ - currentTurn: int                   │                       │
│  │ - characters: map<string, Character> │                       │
│  │ - occupancy: map<HexCoord, string>   │                       │
│  │ - terrain: map<HexCoord, Terrain>    │                       │
│  └───────────┬──────────────────────────┘                       │
│              │ contains                                          │
│              ▼                                                   │
│  Character                  Skill                                │
│  ┌───────────────────┐     ┌──────────────┐                     │
│  │ - name: string    │     │ - id: string │                     │
│  │ - stats: Stats    │────►│ - type: Type │                     │
│  │ - skills: Skill[] │     │ - range: int │                     │
│  │ - playerId: int   │     │ - power: int │                     │
│  └───────────────────┘     └──────────────┘                     │
│                                                                  │
│  Action                                                          │
│  ┌──────────────────────────┐                                   │
│  │ - type: ActionType       │                                   │
│  │ - characterId: string    │                                   │
│  │ - target: HexCoord       │                                   │
│  │ - skillId: string        │                                   │
│  └──────────────────────────┘                                   │
│                                                                  │
└─────────────────────────────────────────────────────────────────┘
```

This architecture provides:
- Clean separation of concerns
- Easy testability
- Future extensibility (graphics, AI, etc.)
- Network transparency
- Type safety
