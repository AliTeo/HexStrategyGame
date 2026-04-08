# Playing from Dorm Networks

## 🎓 Dorm Network Challenge

Dormitories have **special network restrictions**:
- ❌ You can't access the router
- ❌ Port forwarding is impossible
- ❌ Multiple NAT layers block incoming connections
- ❌ Your friend can't connect directly to your IP

## ✅ Solution: Use ngrok (FREE Tunneling Service)

**ngrok** creates a public URL that tunnels to your laptop - works from ANY network!

### Step-by-Step Setup:

#### 1. Install ngrok (Arch Linux)

```bash
# Option A: Use AUR
yay -S ngrok

# Option B: Manual install
cd /tmp
wget https://bin.equinox.io/c/bNyj1mQVY4c/ngrok-v3-stable-linux-amd64.tgz
tar xzf ngrok-v3-stable-linux-amd64.tgz
sudo mv ngrok /usr/local/bin/
```

#### 2. Sign up for ngrok (FREE)

1. Go to: https://ngrok.com/signup
2. Create free account
3. Copy your auth token from dashboard

#### 3. Configure ngrok

```bash
ngrok config add-authtoken YOUR_AUTH_TOKEN_HERE
```

#### 4. Start the game with ngrok

```bash
cd HexGame
./start_with_ngrok.sh
```

**You'll see something like:**
```
Forwarding  https://abc-123-def.ngrok-free.app -> http://localhost:3000
```

#### 5. Share the URL

Send your friend: `https://abc-123-def.ngrok-free.app`

**They can play from anywhere in the world!** 🌍

---

## 🚀 Quick Start (All-in-One)

```bash
# One-time setup
sudo pacman -S nodejs npm
cd HexGame/web_client && npm install && cd ..

# Install ngrok (if not installed)
yay -S ngrok
# OR wget and install manually

# Configure ngrok
ngrok config add-authtoken YOUR_TOKEN

# Start everything
./start_with_ngrok.sh
```

---

## 📊 How It Works

```
Friend's Browser → ngrok Cloud → ngrok on your PC → Your Game Server
   (Anywhere)        (Internet)     (Dorm Network)    (localhost)
```

ngrok creates a **secure tunnel** that bypasses all network restrictions!

---

## 🆓 ngrok Free Tier Limits

- ✅ Unlimited connections
- ✅ HTTPS encryption
- ✅ Random URL each time
- ⚠️  Sessions expire after 2 hours (just restart)
- ⚠️  URL changes each time you start

### Want a permanent URL?

**Upgrade to ngrok Pro** ($8/month):
- Fixed URL (like `yourgame.ngrok.app`)
- No time limits
- Custom domains

**Or use alternatives:**

---

## 🔄 Alternative Solutions

### 1. Cloudflare Tunnel (FREE, No Limits!)

```bash
# Install cloudflared
yay -S cloudflared

# Login
cloudflared tunnel login

# Create tunnel
cloudflared tunnel create hexgame

# Start tunnel
cloudflared tunnel --url http://localhost:3000
```

**Advantages:**
- Completely free
- No time limits
- Better for long-term hosting

### 2. localhost.run (No Signup!)

```bash
# Super simple - no installation or signup
ssh -R 80:localhost:3000 nokey@localhost.run
```

Gives you instant public URL!

### 3. Tailscale (Best for Private Gaming)

```bash
# Install Tailscale
sudo pacman -S tailscale

# Start service
sudo systemctl start tailscaled
sudo tailscale up

# Share with friend (they also install Tailscale)
# You get a private network together
```

**Your friend** installs Tailscale, joins your network, and connects to your private IP!

### 4. Playit.gg (Gaming-Focused)

```bash
# Download
wget https://playit.gg/downloads/playit-linux-amd64
chmod +x playit-linux-amd64

# Run
./playit-linux-amd64
```

Free tunneling specifically for games!

### 5. Use a Free Cloud Server

Deploy to **free hosting**:
- **Replit** - Click "New Repl", import GitHub, instant URL
- **Render** - Free tier, permanent hosting
- **Railway** - Free $5/month credits
- **Heroku** - Free dyno hours

---

## 💡 Recommended for Dorms

**Best: ngrok** (Easiest, works immediately)
**Better: Cloudflare Tunnel** (Free forever, better long-term)
**Advanced: Tailscale** (Private, secure, fast)

---

## 🎮 Testing ngrok Locally First

```bash
# Terminal 1: Start services
./start_with_ngrok.sh

# Terminal 2: Test from your own PC
# Use the ngrok URL in your browser
# Open 2 tabs with the same URL to test multiplayer
```

---

## 🐛 Troubleshooting

**"ERR_NGROK_108" or auth error:**
```bash
ngrok config add-authtoken YOUR_TOKEN
```

**ngrok URL not working:**
- Check if services are actually running
- Test `http://localhost:3000` first
- Check ngrok dashboard: https://dashboard.ngrok.com

**Friend sees "Upgrade Required":**
- Make sure HTTP server is on port 3000
- ngrok should tunnel port 3000, not 9000

**"Session expired":**
- Free tier = 2 hour limit
- Just restart: `./start_with_ngrok.sh`
- URL will change (send new one to friend)

---

## 📞 Other Dorm-Specific Tips

### Check if your dorm allows server hosting

Some dorms **block** all incoming connections. Test with:
```bash
# Start a simple server
python3 -m http.server 8000

# Ask friend to visit: http://YOUR_DORM_IP:8000
```

If that doesn't work → **Use ngrok or cloud hosting**

### Use Dorm VPN (If Available)

Some universities provide VPN that gives you a public IP!

### Ask IT Department

Some dorms can **whitelist** your MAC address for port forwarding.

---

## ✨ Summary for Dorm Networks

**Can't use traditional port forwarding → Use ngrok/tunneling instead!**

| Method | Cost | Setup Time | Best For |
|--------|------|------------|----------|
| **ngrok** | Free | 2 min | Quick testing |
| **Cloudflare Tunnel** | Free | 5 min | Long-term use |
| **Tailscale** | Free | 5 min | Private games |
| **Cloud Hosting** | Free tier | 10 min | Always online |

**Start with ngrok - it's the easiest!**

```bash
./start_with_ngrok.sh
```

That's it! 🎉
