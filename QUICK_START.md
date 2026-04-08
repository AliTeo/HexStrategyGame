# 🎮 Quick Reference: Playing with Your Friend

## 🌐 Web Version (RECOMMENDED - Works on ANY device!)

### You (Host):
```bash
./start_web_server.sh
```

### Your Friend (Player):
1. Open link you send: `http://YOUR_IP:8080`
2. Click "Connect"
3. Play!

**Advantages**: Works on Windows, Mac, Linux, phones, tablets - ANYWHERE!

---

## 💻 Desktop Version (Windows)

### You (Host):
```bash
# Option A: GitHub Actions (Automatic)
git push  # GitHub builds Windows version
# Download artifact from Actions tab

# Option B: Build on Windows
build_windows.bat
# Share the windows_package folder
```

### Your Friend (Windows):
1. Extract ZIP you sent
2. Double-click `HexGameLauncher.exe`
3. Enter your IP
4. Play!

---

## 📋 Quick Commands

| Task | Command |
|------|---------|
| Build everything | `./build.sh` |
| Start web server | `./start_web_server.sh` |
| Test locally | `./test_web_client.sh` |
| Get your IP | `curl ifconfig.me` |
| Build Windows | `build_windows.bat` (on Windows) |

---

## 🔌 Port Forwarding

Forward these ports in your router:
- **8080** - Web client
- **12345** - Game server (optional, web client uses 8080)

Or use **ngrok** for instant public URL:
```bash
ngrok http 8080
```

---

## 🆘 Quick Troubleshooting

**"Connection failed"**
```bash
# Check if services are running
ps aux | grep HexGameServer
ps aux | grep bridge.js

# Restart everything
./start_web_server.sh
```

**"Can't access from friend's PC"**
```bash
# Check firewall
sudo ufw allow 8080

# Test locally first
curl http://localhost:8080

# Verify port forwarding in router
```

**"GitHub Actions build failed"**
- Check Actions tab for logs
- Usually dependency issues
- Workflow auto-retries

---

## 🚀 What You Just Got

✅ **GitHub Actions** - Automatic Windows builds
- File: `.github/workflows/build.yml`
- Builds on every push
- Downloadable packages

✅ **Web Client** - Zero-setup for players
- Folder: `web_client/`
- Beautiful UI
- Works anywhere

✅ **GUI Launcher** - Windows friendly
- File: `client/src/gui_launcher.cpp`
- Added to CMakeLists.txt

✅ **One-Command Scripts**
- `./start_web_server.sh` - Start everything
- `./test_web_client.sh` - Test locally
- `build_windows.bat` - Build for Windows

---

## 🎯 Recommended Workflow

1. **Test locally**:
   ```bash
   ./test_web_client.sh
   # Open http://localhost:8080 in 2 browser tabs
   ```

2. **Play with friend**:
   ```bash
   ./start_web_server.sh
   # Send friend: http://YOUR_IP:8080
   ```

3. **Deploy permanently** (optional):
   - Use Replit for instant hosting
   - Or ngrok for quick sharing
   - Or cloud service (Heroku, AWS, etc.)

---

## 📖 Full Documentation

- `SETUP_COMPLETE.md` - Complete guide
- `web_client/README.md` - Web client details  
- `EASY_SETUP.md` - All setup options
- `WINDOWS_BUILD.md` - Windows packaging

---

## 🎉 You're Ready!

**Simplest way to play right now:**

1. Run: `./test_web_client.sh`
2. Open 2 browser tabs: `http://localhost:8080`
3. Play against yourself to test!

**To play with friend:**

1. Run: `./start_web_server.sh`
2. Get your IP: `curl ifconfig.me`
3. Send friend: `http://YOUR_IP:8080`
4. They click link → instant play!

**No compilation needed for your friend. No setup. Just click and play! 🚀**
