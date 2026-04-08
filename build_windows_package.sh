#!/bin/bash
# Cross-compile Windows package from Linux
# Requires: mingw-w64

echo "=========================================="
echo "  Building Windows Package"
echo "=========================================="

# Set up MinGW compiler
MINGW_PREFIX="x86_64-w64-mingw32"
CXX="${MINGW_PREFIX}-g++"
WINDRES="${MINGW_PREFIX}-windres"

# Create build directory
mkdir -p build_windows
cd build_windows

# Check if MinGW is installed
if ! command -v ${CXX} &> /dev/null; then
    echo "Error: MinGW-w64 not found!"
    echo "Install with: sudo apt-get install mingw-w64"
    exit 1
fi

# Use CMake with MinGW toolchain
echo "Configuring with CMake for Windows..."
cmake .. -DCMAKE_TOOLCHAIN_FILE=../cmake/mingw-toolchain.cmake \
         -DCMAKE_BUILD_TYPE=Release

echo "Building..."
make -j$(nproc)

# Create package directory
echo "Creating Windows package..."
mkdir -p ../windows_package
cp HexGameClient.exe ../windows_package/
cp HexGameLauncher.exe ../windows_package/ 2>/dev/null || echo "Launcher not built"

# Copy required DLLs (adjust paths as needed)
echo "Copying dependencies..."
# Add SFML and other DLLs here

echo ""
echo "=========================================="
echo "  Windows package created!"
echo "=========================================="
echo "Location: windows_package/"
echo ""
echo "Share this folder with your friend."
echo "They just need to:"
echo "  1. Double-click HexGameLauncher.exe"
echo "  2. Enter your IP address"
echo "  3. Click Connect"
echo "=========================================="
