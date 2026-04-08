#!/bin/bash

# Test script to verify SFML installation

echo "Building SFML test..."

SFML_DIR="./external/SFML/install"

if [ ! -d "$SFML_DIR" ]; then
    echo "ERROR: SFML not found at $SFML_DIR"
    echo "Please run ./setup_sfml.sh first"
    exit 1
fi

g++ -std=c++17 test_sfml.cpp \
    -I${SFML_DIR}/include \
    -L${SFML_DIR}/lib \
    -lsfml-graphics-s -lsfml-window-s -lsfml-system-s \
    -lX11 -lXrandr -lXcursor -lXi -lfreetype -lGL -ludev \
    -o test_sfml

if [ $? -eq 0 ]; then
    echo "✓ Build successful!"
    echo "Run with: ./test_sfml"
else
    echo "✗ Build failed!"
    exit 1
fi
