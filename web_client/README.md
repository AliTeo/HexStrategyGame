# Web Client for Hex Strategy Game

Play the game directly in your browser - no installation needed!

## For Your Friend (Player)

### Ultra Simple - Just Open a Link!

1. You send them a link: `http://YOUR_IP:8080`
2. They open it in their browser
3. Game starts automatically!

That's it! No downloads, no setup, no compilation needed.

## For You (Server Host)

### Setup (One Time Only)

```bash
cd web_client

# Install Node.js if needed
# On Ubuntu/Debian:
sudo apt-get install nodejs npm

# Install dependencies
npm install
```

### Running the Web Server

```bash
# Terminal 1: Start the C++ game server
cd ..
./bin/HexGameServer

# Terminal 2: Start the WebSocket bridge
cd web_client
npm start

# Terminal 3: Serve the web client
npx http-server -p 8080 -c-1
```

Or run everything at once:
```bash
npm run dev
```

### Share with Your Friend

1. Find your public IP:
   ```bash
   curl ifconfig.me
   ```

2. Forward port **8080** in your router

3. Send your friend the link:
   ```
   http://YOUR_PUBLIC_IP:8080
   ```

4. They open it in Chrome, Firefox, or any modern browser

5. Done! They're playing!

## Architecture

```
Browser (Friend) <--WebSocket--> Bridge <--TCP--> C++ Server (You)
```

The bridge converts WebSocket protocol (used by browsers) to TCP (used by the C++ server).

## Features

- ✅ No installation for players
- ✅ Works on any device (Windows, Mac, Linux, even mobile!)
- ✅ Beautiful UI with hex grid visualization
- ✅ Real-time multiplayer
- ✅ Same game logic as desktop version

## Advanced: Deploy to Cloud

Host on free services so your friend can always connect:

### Option 1: Replit (Easiest)
1. Sign up at replit.com
2. Create new Repl
3. Upload web_client folder
4. Run `npm start`
5. Share the generated URL

### Option 2: Heroku
```bash
# Add Procfile
echo "web: node bridge.js" > Procfile

# Deploy
heroku create hexgame
git push heroku main
```

### Option 3: Vercel/Netlify
Deploy the static files (index.html, style.css, game.js) and the bridge separately.

## Troubleshooting

**"Connection failed"**
- Make sure C++ server is running
- Check that bridge.js is running
- Verify port 8080 is forwarded

**"Can't load page"**
- Run `npx http-server -p 8080` in web_client folder
- Check firewall settings

**"WebSocket error"**
- Make sure bridge.js is running
- Check TCP_HOST and TCP_PORT in bridge.js match your server

## Development

Edit files:
- `index.html` - Page structure
- `style.css` - Styling
- `game.js` - Game logic and WebSocket client
- `bridge.js` - WebSocket<->TCP bridge

The web client automatically updates when you refresh the browser.

## Future Enhancements

- [ ] Mobile-optimized controls
- [ ] Touch gestures for hex selection
- [ ] Animated battle effects
- [ ] Sound effects
- [ ] Chat system
- [ ] Spectator mode
- [ ] Multiple game rooms
- [ ] PWA support (install as app)

## Browser Compatibility

Tested on:
- Chrome 90+
- Firefox 88+
- Safari 14+
- Edge 90+
- Mobile browsers (iOS Safari, Chrome Android)

## Security Note

For production use:
- Use WSS (WebSocket Secure) with TLS
- Add authentication
- Rate limiting
- Input validation on bridge

For playing with friends, the current setup is fine!
