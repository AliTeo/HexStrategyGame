#!/bin/bash
# One-command startup for web-based gameplay

echo "=========================================="
echo "  Starting Hex Strategy Game Web Server"
echo "=========================================="
echo ""

# Check if Node.js is installed
if ! command -v node &> /dev/null; then
    echo "❌ Node.js not found!"
    echo "Install with: sudo apt-get install nodejs npm"
    exit 1
fi

# Check if game server is built
if [ ! -f "./bin/HexGameServer" ]; then
    echo "❌ Game server not built!"
    echo "Run: ./build.sh"
    exit 1
fi

# Install web dependencies if needed
if [ ! -d "./web_client/node_modules" ]; then
    echo "📦 Installing web dependencies..."
    cd web_client
    npm install
    cd ..
fi

# Get local IP
LOCAL_IP=$(hostname -I | awk '{print $1}')
PUBLIC_IP=$(curl -s ifconfig.me || echo "unknown")

echo "✅ Ready to start!"
echo ""
echo "Local IP:  $LOCAL_IP"
echo "Public IP: $PUBLIC_IP"
echo ""
echo "Share this with your friend:"
echo "  http://$PUBLIC_IP:8080"
echo ""
echo "Don't forget to forward port 8080 in your router!"
echo ""
echo "=========================================="
echo ""

# Start everything in background
echo "Starting C++ game server..."
./bin/HexGameServer &
SERVER_PID=$!

sleep 2

echo "Starting WebSocket bridge..."
cd web_client
node bridge.js &
BRIDGE_PID=$!

sleep 1

echo "Starting HTTP server for web client..."
npx http-server -p 8080 -c-1 --silent &
HTTP_PID=$!

cd ..

echo ""
echo "=========================================="
echo "  🎮 ALL SYSTEMS RUNNING!"
echo "=========================================="
echo ""
echo "You can now connect at:"
echo "  Local:  http://localhost:8080"
echo "  Friend: http://$PUBLIC_IP:8080"
echo ""
echo "Press Ctrl+C to stop all servers"
echo ""

# Trap Ctrl+C to clean up
trap "echo ''; echo 'Stopping servers...'; kill $SERVER_PID $BRIDGE_PID $HTTP_PID 2>/dev/null; echo 'Done!'; exit" INT

# Wait forever
wait
