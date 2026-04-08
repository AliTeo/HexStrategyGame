# Compiling SFML Client to WebAssembly

## Overview

You can compile the existing SFML C++ client to run in the browser using **Emscripten**. This gives you:
- ✅ Same graphics as desktop version
- ✅ Same game logic
- ✅ Runs in browser (no installation for friend)
- ✅ Works on any OS

## Prerequisites

### 1. Install Emscripten

```bash
# On Arch Linux
yay -S emscripten

# Or manual install
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk
./emsdk install latest
./emsdk activate latest
source ./emsdk_env.sh
```

### 2. Get SFML for Emscripten

SFML has experimental WebAssembly support. You'll need SFML 3.0+ or a compatible version.

```bash
# Clone SFML
git clone https://github.com/SFML/SFML.git external/SFML
cd external/SFML
git checkout 3.0.0
```

## Building SFML for WebAssembly

```bash
# Create build directory
mkdir build-wasm
cd build-wasm

# Configure with Emscripten
emcmake cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_SHARED_LIBS=OFF \
  -DSFML_BUILD_NETWORK=OFF \
  -DSFML_BUILD_AUDIO=ON

# Build
emmake make -j$(nproc)

# Install
make install
```

## Modify Client for WebAssembly

The client needs modifications for the browser environment:

### 1. Use Emscripten's Main Loop

```cpp
// Instead of while(window.isOpen())
#ifdef __EMSCRIPTEN__
#include <emscripten.h>

void mainLoop() {
    // Game loop iteration
}

int main() {
    // Setup
    emscripten_set_main_loop(mainLoop, 0, 1);
}
#else
    // Normal desktop loop
#endif
```

### 2. Use WebSocket Instead of TCP

Emscripten provides WebSocket API:

```cpp
#ifdef __EMSCRIPTEN__
#include <emscripten/websocket.h>
// Use WebSocket
#else
// Use ASIO
#endif
```

## Build Script

I'll create a build script that compiles to WebAssembly:

```bash
#!/bin/bash
# build_web.sh

emcc client/src/*.cpp common/src/*.cpp \
  -o web_client/game.html \
  -s USE_SDL=2 \
  -s USE_WEBGL2=1 \
  -s FULL_ES3=1 \
  -s WASM=1 \
  -s ALLOW_MEMORY_GROWTH=1 \
  -s EXPORTED_RUNTIME_METHODS=['ccall','cwrap'] \
  -I client/include \
  -I common/include \
  -I external/SFML/include \
  -L external/SFML/build-wasm/lib \
  -lsfml-graphics -lsfml-window -lsfml-system \
  --preload-file assets
```

## Pros & Cons

### ✅ Advantages
- Exact same graphics as desktop
- Same C++ codebase
- Professional looking
- Native performance (WebAssembly is fast)

### ❌ Challenges
- Complex build setup
- Larger file size (~5-10 MB)
- Initial load time
- Requires modifications to client code
- Network code needs WebSocket wrapper
- Some SFML features may not work in browser

## ⚡ Quick Alternative: Keep Simple Web Client

The JavaScript/Canvas web client I created is:
- ✅ Works immediately (no complex build)
- ✅ Much smaller file size
- ✅ Faster loading
- ✅ Easier to debug/modify
- ❌ Different graphics than SFML

## 🎯 Recommendation

### Option A: Two-Tier System (Best of Both)

1. **Quick Web Client** (current) - For instant access
2. **Windows Package** - For full SFML experience

Your friend chooses:
- Web link → Quick play with basic graphics
- Download package → Full graphics experience

### Option B: Full Emscripten Build

If you want **only one version** with SFML graphics in browser:
- Takes ~2-3 hours to set up properly
- Requires modifying client code
- Complex debugging
- Worth it for production/polish

### Option C: Improve Current Web Client

Make the JavaScript version look closer to SFML:
- Add animations
- Better hex rendering
- Particle effects
- Much faster than Emscripten setup

## My Recommendation

**Start with Option A** (two-tier):
- Friend uses web client for quick play
- Can download Windows package for full experience
- You get both working right now

**Then later** (if needed):
- Build Emscripten version for production
- But only if many people will play

## Want Me To:

1. **Set up Emscripten build** (2-3 hours of work, complex)
2. **Improve current web graphics** (1 hour, easier)
3. **Focus on Windows package** (works now, full graphics)

Which would you prefer?
