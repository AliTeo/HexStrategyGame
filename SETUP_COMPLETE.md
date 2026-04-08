# GitHub Actions + Web Client Setup Complete! 🎉

## ✅ What's Been Created

### 1. GitHub Actions (Automated Windows Builds)
- **Location**: `.github/workflows/build.yml`
- **What it does**: 
  - Automatically builds Windows executables when you push code
  - Creates downloadable packages
  - Builds both Windows and Linux versions

### 2. Web Client (Zero Setup for Players)
- **Location**: `web_client/`
- **What it includes**:
  - `index.html` - Beautiful game interface
  - `style.css` - Modern, responsive design
  - `game.js` - Full game client in browser
  - `bridge.js` - WebSocket-to-TCP bridge
  - `README.md` - Complete documentation

## 🚀 Quick Start

### Option A: Windows Package via GitHub Actions

1. **Push your code to GitHub**:
   ```bash
   git init
   git add .
   git commit -m "Add Windows build and web client"
   git branch -M main
   git remote add origin https://github.com/YOUR_USERNAME/HexGame.git
   git push -u origin main
   ```

2. **GitHub automatically builds Windows package**
   - Go to your repo → Actions tab
   - Wait for build to complete (~5 minutes)
   - Download the artifact: `HexGame-Windows-{hash}.zip`

3. **Share with your friend**:
   - Upload the ZIP to Google Drive/Dropbox
   - Send them the link
   - They extract and double-click `HexGameLauncher.exe`

### Option B: Web Client (Even Easier!)

1. **One-time setup** (on your machine):
   ```bash
   cd web_client
   npm install
   cd ..
   ```

2. **Start everything with one command**:
   ```bash
   ./start_web_server.sh
   ```

3. **Share the URL**:
   ```bash
   # The script shows you this automatically
   http://YOUR_PUBLIC_IP:8080
   ```

4. **Your friend**:
   - Opens the link in ANY browser
   - Game loads automatically
   - Clicks "Connect to Game"
   - Plays instantly!

## 📊 Comparison

| Method | Friend's Setup | Your Setup | Best For |
|--------|----------------|------------|----------|
| **Windows Package** | Download ZIP, extract, double-click | Build once via GitHub | Windows users |
| **Web Client** | Click link | Run `./start_web_server.sh` | ANYONE with a browser |

## 🎯 Recommended: Use Web Client

The web client is superior because:
- ✅ Works on **any device** (Windows, Mac, Linux, even phones!)
- ✅ **Zero installation** for your friend
- ✅ **Instant updates** - just refresh browser
- ✅ **Easier to share** - just a URL
- ✅ **Beautiful UI** out of the box

## 📝 Step-by-Step: Web Client

### You (Server Host):

1. **Build the game** (if not already built):
   ```bash
   ./build.sh
   ```

2. **Install Node.js** (if not installed):
   ```bash
   sudo apt-get install nodejs npm
   ```

3. **Setup web client** (one time):
   ```bash
   cd web_client
   npm install
   cd ..
   ```

4. **Start the server**:
   ```bash
   ./start_web_server.sh
   ```

5. **Get your public IP**:
   ```bash
   curl ifconfig.me
   # Example: 123.45.67.89
   ```

6. **Forward port 8080** in your router:
   - Open router admin (usually 192.168.1.1)
   - Add port forwarding rule: External 8080 → Internal 8080 → Your PC's local IP

7. **Send this to your friend**:
   ```
   http://123.45.67.89:8080
   ```

### Your Friend (Player):

1. **Click the link you sent**
2. **Page loads with the game**
3. **Click "Connect to Game"**
4. **Select characters and play!**

That's literally it! 🎮

## 🔧 Manual Start (If Script Doesn't Work)

```bash
# Terminal 1: Game server
./bin/HexGameServer

# Terminal 2: WebSocket bridge
cd web_client
node bridge.js

# Terminal 3: Web server
cd web_client
npx http-server -p 8080 -c-1
```

## 🌐 Deploy to Cloud (Advanced)

For permanent hosting (no port forwarding needed):

### Replit (Easiest - 5 minutes):
1. Go to replit.com
2. Create new Repl → Import from GitHub
3. Select your repository
4. Run button → automatically hosts
5. Share the generated URL

### Heroku (Free):
```bash
cd web_client
echo "web: node bridge.js" > Procfile
heroku create hexgame-yourname
git push heroku main
```

### ngrok (Quick Tunneling):
```bash
# No router config needed!
ngrok http 8080
# Gives you public URL instantly
```

## 🐛 Troubleshooting

**Web client shows "Connection failed":**
- Check game server is running: `ps aux | grep HexGameServer`
- Check bridge is running: `ps aux | grep bridge.js`
- Check ports: `netstat -tulpn | grep -E '8080|12345'`

**Friend can't access the link:**
- Verify port 8080 is forwarded in router
- Check firewall: `sudo ufw allow 8080`
- Test locally first: `http://localhost:8080`

**GitHub Actions build fails:**
- Check the Actions tab for error logs
- Common issue: dependencies not found
- Workflow will retry automatically

## 📚 Documentation

- `web_client/README.md` - Web client details
- `.github/workflows/build.yml` - Build configuration
- `EASY_SETUP.md` - All setup methods
- `WINDOWS_BUILD.md` - Windows-specific guide

## 🎉 You're All Set!

You now have:
1. ✅ **Automated Windows builds** via GitHub Actions
2. ✅ **Browser-based client** that works anywhere
3. ✅ **One-command startup** script
4. ✅ **Complete documentation**

Just run `./start_web_server.sh` and send your friend the link!

**Next game night is sorted! 🎮⬡**
