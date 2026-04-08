#!/bin/bash

# Build script for Hexagonal Strategy Game
set -euo pipefail

echo "Building Hexagonal Strategy Game..."
echo "===================================="

# Compiler settings
CXX=g++
CXXFLAGS="-std=c++17 -Wall -Wextra -O2"
INCLUDES="-I./common/include -I./external/json/include"
LIBS="-pthread"

# SFML settings
SFML_DIR="./external/SFML/install"
SFML_INCLUDES="-I${SFML_DIR}/include"
SFML_LIBS="-L${SFML_DIR}/lib -lsfml-graphics-s -lsfml-window-s -lsfml-system-s -lsfml-audio-s"
SFML_DEPS="-lX11 -lXrandr -lXcursor -lXi -lfreetype -lGL -ludev -lopenal -lvorbisenc -lvorbisfile -lvorbis -logg -lFLAC"

# Check if SFML is built
if [ ! -d "$SFML_DIR" ]; then
    echo ""
    echo "ERROR: SFML not found!"
    echo "Please run ./setup_sfml.sh first to download and build SFML 3.0.2"
    exit 1
fi

# Output directories
mkdir -p bin
mkdir -p build/common
mkdir -p build/server
mkdir -p build/client
mkdir -p build/text_client

# Clean stale objects so target layouts can't mix (e.g., text/main.o vs sfml_main.o)
rm -f build/common/*.o build/server/*.o build/client/*.o build/text_client/*.o

echo "Compiling common library..."
$CXX $CXXFLAGS $INCLUDES -c common/src/HexCoord.cpp -o build/common/HexCoord.o
$CXX $CXXFLAGS $INCLUDES -c common/src/Character.cpp -o build/common/Character.o
$CXX $CXXFLAGS $INCLUDES -c common/src/Skill.cpp -o build/common/Skill.o
$CXX $CXXFLAGS $INCLUDES -c common/src/Action.cpp -o build/common/Action.o
$CXX $CXXFLAGS $INCLUDES -c common/src/GameState.cpp -o build/common/GameState.o
$CXX $CXXFLAGS $INCLUDES -c common/src/Protocol.cpp -o build/common/Protocol.o

COMMON_OBJS="build/common/HexCoord.o build/common/Character.o build/common/Skill.o build/common/Action.o build/common/GameState.o build/common/Protocol.o"

echo "Compiling server..."
$CXX $CXXFLAGS $INCLUDES -I./server/include -c server/src/TurnResolver.cpp -o build/server/TurnResolver.o
$CXX $CXXFLAGS $INCLUDES -I./server/include -c server/src/GameSession.cpp -o build/server/GameSession.o
$CXX $CXXFLAGS $INCLUDES -I./server/include -c server/src/MatchManager.cpp -o build/server/MatchManager.o
$CXX $CXXFLAGS $INCLUDES -I./server/include -c server/src/GameServer.cpp -o build/server/GameServer.o
$CXX $CXXFLAGS $INCLUDES -I./server/include -c server/src/main.cpp -o build/server/main.o

echo "Linking server..."
$CXX build/server/*.o $COMMON_OBJS $LIBS -o bin/HexGameServer

echo "Compiling SFML client..."
$CXX $CXXFLAGS $INCLUDES $SFML_INCLUDES -I./client/include -c client/src/HexRenderer.cpp -o build/client/HexRenderer.o
$CXX $CXXFLAGS $INCLUDES $SFML_INCLUDES -I./client/include -c client/src/SFMLGameClient.cpp -o build/client/SFMLGameClient.o
$CXX $CXXFLAGS $INCLUDES $SFML_INCLUDES -I./client/include -c sfml_main.cpp -o build/client/sfml_main.o

echo "Linking SFML client..."
$CXX build/client/*.o $COMMON_OBJS $LIBS $SFML_LIBS $SFML_DEPS -o bin/HexGameClient

echo "Compiling text client..."
$CXX $CXXFLAGS $INCLUDES -I./client/include -c client/src/TextRenderer.cpp -o build/text_client/TextRenderer.o
$CXX $CXXFLAGS $INCLUDES -I./client/include -c client/src/InputHandler.cpp -o build/text_client/InputHandler.o
$CXX $CXXFLAGS $INCLUDES -I./client/include -c client/src/MenuSystem.cpp -o build/text_client/MenuSystem.o
$CXX $CXXFLAGS $INCLUDES -I./client/include -c client/src/GameClient.cpp -o build/text_client/GameClient.o
$CXX $CXXFLAGS $INCLUDES -I./client/include -c client/src/main.cpp -o build/text_client/main.o

echo "Linking text client..."
$CXX build/text_client/*.o $COMMON_OBJS $LIBS -o bin/HexGameClientText

echo ""
echo "Build complete!"
echo "Server: ./bin/HexGameServer"
echo "SFML Client: ./bin/HexGameClient"
echo "Text Client: ./bin/HexGameClientText"
