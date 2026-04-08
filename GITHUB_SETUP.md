# Quick Guide: Get Windows Package Built

## Step 1: Initialize Git Repository

```bash
cd /home/aliteo34/Documents/Projects/HexagonalMultiplayerCPP/HexGame

# Initialize git
git init

# Add all files
git add .

# Create first commit
git commit -m "Initial commit with Windows build support"
```

## Step 2: Create GitHub Repository

1. Go to: https://github.com/new
2. Name it: `HexStrategyGame` (or whatever you like)
3. **Don't** initialize with README (we already have files)
4. Click "Create repository"

## Step 3: Push to GitHub

```bash
# Add GitHub as remote (replace YOUR_USERNAME)
git remote add origin https://github.com/YOUR_USERNAME/HexStrategyGame.git

# Push code
git branch -M main
git push -u origin main
```

## Step 4: GitHub Actions Automatically Builds!

- Go to your repo → **Actions** tab
- Wait 5-10 minutes for build to complete
- You'll see: ✅ Build Windows Release

## Step 5: Download Windows Package

1. Click on the completed workflow
2. Scroll down to **Artifacts**
3. Download: `HexGame-Windows-{hash}.zip`
4. Extract it

## Step 6: Share with Friend

**Option A: Direct File Share**
```bash
# Upload the ZIP to:
- Google Drive
- Dropbox
- WeTransfer
- Discord/Telegram
```

**Option B: GitHub Release (Better)**
```bash
# Create a release tag
git tag v1.0.0
git push origin v1.0.0

# GitHub Actions automatically creates release
# Friend downloads from Releases page
```

## What Your Friend Gets

```
HexGame-Windows/
├── HexGameLauncher.exe    ← Double-click this!
├── HexGameClient.exe
├── HexGameServer.exe
├── sfml-graphics-3.dll
├── sfml-window-3.dll
├── sfml-system-3.dll
├── sfml-audio-3.dll
└── README.txt
```

## Your Friend's Steps

1. Extract ZIP
2. Double-click `HexGameLauncher.exe`
3. Enter your IP address (from ngrok or your public IP)
4. Click "Connect"
5. Play with full SFML graphics! 🎮

---

## Alternative: Build Locally on Windows

If you have access to a Windows machine:

```cmd
cd HexGame
build_windows.bat
```

Package will be in `windows_package/` folder.

---

## Troubleshooting GitHub Actions

**Build fails?**
- Check Actions tab for error logs
- Common issue: dependencies not found
- SFML 3.0 might need adjustment

**No Windows build showing?**
- Make sure `.github/workflows/build.yml` is committed
- Check Actions tab is enabled in repo settings

**Want to trigger build manually?**
- Go to Actions tab
- Select "Build Windows Release"
- Click "Run workflow"

---

## Current Status

✅ GitHub Actions workflow created (`.github/workflows/build.yml`)
✅ GUI Launcher code written (`client/src/gui_launcher.cpp`)
✅ CMakeLists.txt configured for Windows
✅ Build scripts ready (`build_windows.bat`)

**Next step: Push to GitHub!**

---

## For Server Hosting (You)

While your friend uses Windows package, you run:

```bash
# Option 1: Simple (local network)
./bin/HexGameServer

# Option 2: With ngrok (internet play)
./bin/HexGameServer &
ngrok tcp 12345
```

Give friend the ngrok address!

---

## Summary

1. `git init && git add . && git commit -m "Initial"`
2. Create GitHub repo
3. `git push`
4. Wait for build (5-10 min)
5. Download from Actions → Artifacts
6. Share ZIP with friend
7. They extract and double-click launcher
8. Play! 🎉
