# External Dependencies

This directory contains third-party libraries used by the project.

## ASIO (Standalone)

Header-only C++ networking library.

**Download**: https://github.com/chriskohlhoff/asio/releases

```bash
cd external
wget https://github.com/chriskohlhoff/asio/archive/refs/tags/asio-1-28-0.tar.gz
tar -xzf asio-1-28-0.tar.gz
mv asio-asio-1-28-0/asio/include asio
rm -rf asio-asio-1-28-0 asio-1-28-0.tar.gz
```

Or manually:
1. Download ASIO from the link above
2. Extract to `external/asio/include`

## nlohmann/json

Header-only JSON library for Modern C++.

**Download**: https://github.com/nlohmann/json/releases

```bash
cd external
mkdir -p json/include
cd json/include
wget https://raw.githubusercontent.com/nlohmann/json/develop/single_include/nlohmann/json.hpp
mkdir -p nlohmann
mv json.hpp nlohmann/
```

Or manually:
1. Download `json.hpp` from the link above
2. Place in `external/json/include/nlohmann/json.hpp`

## Quick Setup Script

Run this from the `external/` directory:

```bash
#!/bin/bash

# ASIO
mkdir -p asio
cd asio
wget https://github.com/chriskohlhoff/asio/archive/refs/tags/asio-1-28-0.tar.gz
tar -xzf asio-1-28-0.tar.gz
mv asio-asio-1-28-0/asio/include .
rm -rf asio-asio-1-28-0 asio-1-28-0.tar.gz
cd ..

# nlohmann/json
mkdir -p json/include/nlohmann
cd json/include/nlohmann
wget https://raw.githubusercontent.com/nlohmann/json/develop/single_include/nlohmann/json.hpp
cd ../../..

echo "Dependencies downloaded successfully!"
```

## Verification

After setup, you should have:
```
external/
├── asio/
│   └── include/
│       └── asio/
│           └── [headers]
└── json/
    └── include/
        └── nlohmann/
            └── json.hpp
```
