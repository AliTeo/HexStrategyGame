#!/bin/bash
# Easy setup with ngrok for dorm networks

echo "=========================================="
echo "  Dorm-Friendly Game Server Setup"
echo "=========================================="
echo ""

# Check if ngrok is installed
if ! command -v ngrok &> /dev/null; then
    echo "📦 Installing ngrok..."
    
    # Download ngrok for Linux
    echo "Downloading ngrok..."
    wget -q https://bin.equinox.io/c/bNyj1mQVY4c/ngrok-v3-stable-linux-amd64.tgz
    tar xzf ngrok-v3-stable-linux-amd64.tgz
    sudo mv ngrok /usr/local/bin/
    rm ngrok-v3-stable-linux-amd64.tgz
    
    echo "✅ ngrok installed!"
    echo ""
    echo "⚠️  Please sign up at https://ngrok.com (free)"
    echo "Then run: ngrok config add-authtoken YOUR_TOKEN"
    echo ""
    read -p "Press Enter after you've added your auth token..."
fi

# Check if game server is built
if [ ! -f "./bin/HexGameServer" ]; then
    echo "❌ Game server not built! Run: ./build.sh"
    exit 1
fi

# Check if web dependencies are installed
if [ ! -d "./web_client/node_modules" ]; then
    echo "📦 Installing web dependencies..."
    cd web_client && npm install && cd ..
fi

echo "🚀 Starting services..."
echo ""

# Start game server
./bin/HexGameServer > /tmp/hexgame_server.log 2>&1 &
SERVER_PID=$!
echo "✅ Game server started (PID: $SERVER_PID)"
sleep 2

# Start WebSocket bridge
cd web_client
node bridge.js > /tmp/hexgame_bridge.log 2>&1 &
BRIDGE_PID=$!
echo "✅ WebSocket bridge started (PID: $BRIDGE_PID)"
cd ..
sleep 2

# Start HTTP server on port 3000 (ngrok will tunnel this)
cd web_client
npx http-server -p 3000 -c-1 --silent > /tmp/hexgame_http.log 2>&1 &
HTTP_PID=$!
echo "✅ HTTP server started on port 3000 (PID: $HTTP_PID)"
cd ..
sleep 2

echo ""
echo "🌐 Starting ngrok tunnel..."
echo ""

# Start ngrok
ngrok http 3000 &
NGROK_PID=$!

echo ""
echo "=========================================="
echo "  ✨ ALL SYSTEMS RUNNING!"
echo "=========================================="
echo ""
echo "🔍 Look for the 'Forwarding' line above"
echo "It will show something like:"
echo "  https://abc123.ngrok.io"
echo ""
echo "📤 Send that URL to your friend!"
echo ""
echo "They can play from ANYWHERE in the world!"
echo ""
echo "Press Ctrl+C to stop all servers"
echo ""

# Cleanup on exit
trap "echo ''; echo '🛑 Stopping all services...'; kill $SERVER_PID $BRIDGE_PID $HTTP_PID $NGROK_PID 2>/dev/null; echo '✅ Done!'; exit" INT

wait
