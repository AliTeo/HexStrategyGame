#!/bin/bash

# Script to set up SFML 3.0.2

set -e

SFML_VERSION="3.0.2"
SFML_DIR="external/SFML"

echo "Setting up SFML ${SFML_VERSION}..."
echo "===================================="

# Check for cmake
if ! command -v cmake &> /dev/null; then
    echo "ERROR: CMake is not installed!"
    echo "Please install CMake first:"
    echo "  sudo pacman -S cmake"
    exit 1
fi

# Create external directory if it doesn't exist
mkdir -p external

# Check if SFML is already downloaded
if [ ! -d "$SFML_DIR" ]; then
    echo "Downloading SFML ${SFML_VERSION}..."
    cd external
    git clone --depth 1 --branch ${SFML_VERSION} https://github.com/SFML/SFML.git
    cd ..
else
    echo "SFML directory already exists, skipping download..."
fi

# Build SFML
echo "Building SFML (this may take a few minutes)..."
mkdir -p "$SFML_DIR/build"
cd "$SFML_DIR/build"

# Configure with CMake
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_SHARED_LIBS=FALSE \
    -DSFML_BUILD_EXAMPLES=FALSE \
    -DSFML_BUILD_DOC=FALSE \
    -DSFML_BUILD_NETWORK=FALSE \
    -DCMAKE_INSTALL_PREFIX=../install

# Build and install
make -j$(nproc)
make install

cd ../../..

echo ""
echo "✓ SFML ${SFML_VERSION} setup complete!"
echo "Library location: ${SFML_DIR}/install/lib"
echo "Include location: ${SFML_DIR}/install/include"
echo ""
echo "You can now build the project with ./build.sh"

