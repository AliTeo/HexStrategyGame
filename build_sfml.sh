#!/bin/bash

# Build script for SFML Hexagonal Strategy Game Client

echo "Building SFML Hexagonal Strategy Game Client..."
echo "================================================"

# Compiler settings
CXX=g++
CXXFLAGS="-std=c++17 -Wall -Wextra -O2"
INCLUDES="-I./common/include -I./client/include -I./external/json/include"
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
mkdir -p build/sfml_client

echo "Compiling common library..."
$CXX $CXXFLAGS $INCLUDES -c common/src/HexCoord.cpp -o build/common/HexCoord.o
$CXX $CXXFLAGS $INCLUDES -c common/src/Character.cpp -o build/common/Character.o
$CXX $CXXFLAGS $INCLUDES -c common/src/Skill.cpp -o build/common/Skill.o
$CXX $CXXFLAGS $INCLUDES -c common/src/Action.cpp -o build/common/Action.o
$CXX $CXXFLAGS $INCLUDES -c common/src/GameState.cpp -o build/common/GameState.o
$CXX $CXXFLAGS $INCLUDES -c common/src/Protocol.cpp -o build/common/Protocol.o

COMMON_OBJS="build/common/HexCoord.o build/common/Character.o build/common/Skill.o build/common/Action.o build/common/GameState.o build/common/Protocol.o"

echo "Compiling SFML client..."
$CXX $CXXFLAGS $INCLUDES $SFML_INCLUDES -c client/src/HexRenderer.cpp -o build/sfml_client/HexRenderer.o
$CXX $CXXFLAGS $INCLUDES $SFML_INCLUDES -c client/src/SFMLGameClient.cpp -o build/sfml_client/SFMLGameClient.o
$CXX $CXXFLAGS $INCLUDES $SFML_INCLUDES -c sfml_main.cpp -o build/sfml_client/sfml_main.o

echo "Linking SFML client..."
$CXX build/sfml_client/*.o $COMMON_OBJS $LIBS $SFML_LIBS $SFML_DEPS -o bin/HexGameSFML

echo ""
echo "Build complete!"
echo "SFML Client: ./bin/HexGameSFML"
