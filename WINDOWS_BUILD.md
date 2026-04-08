# Windows Package Guide

This guide explains how to create a ready-to-run Windows package for your friend.

## Quick Option: Pre-built Executables

The easiest way is to build on Windows directly or use GitHub Actions to build Windows binaries.

### Option 1: Build on Windows (Recommended)

If you have access to a Windows machine:

```cmd
# Install Visual Studio 2019+ with C++ Desktop Development
# Install CMake

cd HexGame
mkdir build
cd build
cmake ..
cmake --build . --config Release

# Package files
mkdir ..\windows_package
copy Release\HexGameClient.exe ..\windows_package\
copy Release\*.dll ..\windows_package\
```

### Option 2: Cross-Compile from Linux

Install MinGW:
```bash
sudo apt-get install mingw-w64 cmake
```

Create CMake toolchain file:
```bash
# Create cmake/mingw-toolchain.cmake
cat > cmake/mingw-toolchain.cmake << 'EOF'
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)
set(CMAKE_RC_COMPILER x86_64-w64-mingw32-windres)
set(CMAKE_FIND_ROOT_PATH /usr/x86_64-w64-mingw32)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
EOF
```

Build:
```bash
mkdir build_windows && cd build_windows
cmake .. -DCMAKE_TOOLCHAIN_FILE=../cmake/mingw-toolchain.cmake
make
```

### Option 3: Use GitHub Actions (Best for Distribution)

Create `.github/workflows/build-windows.yml`:

```yaml
name: Build Windows Release

on:
  push:
    branches: [ main ]
  workflow_dispatch:

jobs:
  build:
    runs-on: windows-latest
    steps:
      - uses: actions/checkout@v3
      
      - name: Install dependencies
        run: |
          vcpkg install boost-asio:x64-windows
          vcpkg install sfml:x64-windows
      
      - name: Configure CMake
        run: cmake -B build -DCMAKE_BUILD_TYPE=Release
      
      - name: Build
        run: cmake --build build --config Release
      
      - name: Package
        run: |
          mkdir windows_package
          cp build/Release/*.exe windows_package/
          cp build/Release/*.dll windows_package/
      
      - name: Upload artifact
        uses: actions/upload-artifact@v3
        with:
          name: HexGame-Windows
          path: windows_package/
```

## Creating the GUI Launcher

The GUI launcher has been added to the project. To build it:

```bash
# Add to CMakeLists.txt:
add_executable(HexGameLauncher
    client/src/gui_launcher.cpp
)

target_link_libraries(HexGameLauncher PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System
)
```

## Distribution Package Structure

Your friend needs this folder structure:

```
HexGame_Windows/
├── HexGameLauncher.exe    # GUI launcher (double-click this)
├── HexGameClient.exe      # Actual game client
├── sfml-graphics-3.dll    # SFML dependencies
├── sfml-window-3.dll
├── sfml-system-3.dll
├── sfml-audio-3.dll
└── README.txt             # Simple instructions
```

## Simple README.txt for Your Friend

```
HOW TO PLAY:
============

1. Double-click "HexGameLauncher.exe"
2. Enter the server IP address (ask your friend)
3. Keep the port as 12345 (unless told otherwise)
4. Click "Connect"

That's it! The game will open in a new window.

CONTROLS:
- Type commands in the console window that opens
- Follow the on-screen instructions
- Type 'help' to see available commands

TROUBLESHOOTING:
- Make sure your friend's server is running
- Check that you entered the correct IP address
- Try disabling your firewall temporarily
- Contact your friend if connection fails
```

## Packaging Script

Create a simple batch file `package.bat` for Windows:

```batch
@echo off
echo Creating Windows Package...

mkdir windows_package
copy build\Release\HexGameClient.exe windows_package\
copy build\Release\HexGameLauncher.exe windows_package\
copy build\Release\*.dll windows_package\

echo.
echo Package created in windows_package/
echo.
echo Zip this folder and send to your friend!
pause
```

## Making it Even Simpler

For the absolute easiest distribution:

1. **Create an installer with Inno Setup** (free Windows installer creator)
2. **Or use a portable ZIP** - just compress the windows_package folder
3. **Host on file sharing** - Upload to Google Drive, Dropbox, etc.

## Security Note

When sharing executables:
- Let your friend know it's from you
- Consider signing the executable
- Or share the source code so they can verify
- Use GitHub Releases for public distribution

## Future: Web Version

For the ultimate simplicity, consider making a web-based client using:
- Emscripten (compile C++ to WebAssembly)
- Or rewrite client in JavaScript
- Friend just opens a URL - no download needed!
