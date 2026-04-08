# Quick Setup for Click-and-Play Windows Experience

## For You (Server Host)

### 1. Start Your Server
```bash
cd HexGame
./bin/HexGameServer
```

### 2. Find Your Public IP
```bash
curl ifconfig.me
```
Share this IP with your friend.

### 3. Forward Port 12345
- Open your router settings (usually 192.168.1.1)
- Forward port **12345** to your computer's local IP
- Or use services like **ngrok**: `ngrok tcp 12345`

## For Your Friend (Windows Player)

### You Need to Create the Package First

**Option A: On Windows (Easiest)**
1. Get access to any Windows PC
2. Copy the project folder to it
3. Double-click `build_windows.bat`
4. Share the `windows_package` folder

**Option B: Using Cloud Build (No Windows Needed)**
1. Push code to GitHub
2. GitHub Actions will build Windows version
3. Download the artifact
4. Share with friend

**Option C: Cross-compile from Linux**
```bash
sudo apt-get install mingw-w64
./build_windows_package.sh
```

### What Your Friend Gets

A folder with:
- **HexGameLauncher.exe** ← They double-click this
- HexGameClient.exe
- Required DLLs
- README.txt

### How Your Friend Connects

1. Extract the ZIP you send them
2. Double-click **HexGameLauncher.exe**
3. A window opens with two input fields:
   - **Server IP**: They type YOUR public IP (you tell them)
   - **Port**: Leave as 12345
4. Click **Connect** button
5. Game starts automatically!

## Even Simpler: No Launcher Option

If building the launcher is too complex, your friend can:

1. Extract the package
2. Open Command Prompt in that folder
3. Type: `HexGameClient.exe YOUR.IP.HERE 12345`
4. Press Enter

Still just 3 steps!

## Simplest: Use a Cloud Gaming Service

Deploy your server on:
- **Heroku** (free tier)
- **Replit** (instant hosting)
- **AWS/Azure** free tier

Give your friend a permanent IP/domain that always works.

## Need Help Building?

I can help you:
1. Set up GitHub Actions for automatic Windows builds
2. Create a proper installer with Inno Setup
3. Or build a web version (no download at all!)

Which would you prefer?
