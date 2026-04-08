#!/bin/bash
# Super simple test of the web client locally

echo "🧪 Testing Web Client Locally"
echo ""
echo "This will:"
echo "  1. Check if everything is installed"
echo "  2. Start all services"
echo "  3. Open browser to test"
echo ""

# Check Node.js
if command -v node &> /dev/null; then
    echo "✅ Node.js found: $(node --version)"
else
    echo "❌ Node.js not found. Install with:"
    echo "   sudo apt-get install nodejs npm"
    exit 1
fi

# Check game server
if [ -f "./bin/HexGameServer" ]; then
    echo "✅ Game server built"
else
    echo "❌ Game server not built. Run:"
    echo "   ./build.sh"
    exit 1
fi

# Install web deps
if [ ! -d "./web_client/node_modules" ]; then
    echo "📦 Installing dependencies..."
    cd web_client && npm install && cd ..
fi

echo ""
echo "🚀 Starting services..."
echo ""

# Start game server
./bin/HexGameServer > /tmp/hexgame_server.log 2>&1 &
SERVER_PID=$!
echo "✅ Game server started (PID: $SERVER_PID)"

sleep 2

# Start bridge
cd web_client
node bridge.js > /tmp/hexgame_bridge.log 2>&1 &
BRIDGE_PID=$!
echo "✅ WebSocket bridge started (PID: $BRIDGE_PID)"
cd ..

sleep 2

# Start http server
cd web_client
npx http-server -p 8080 -c-1 --silent > /tmp/hexgame_http.log 2>&1 &
HTTP_PID=$!
echo "✅ Web server started (PID: $HTTP_PID)"
cd ..

sleep 2

echo ""
echo "🎉 All services running!"
echo ""
echo "Open your browser to:"
echo "  http://localhost:8080"
echo ""
echo "Or from another device on your network:"
echo "  http://$(hostname -I | awk '{print $1}'):8080"
echo ""

# Try to open browser
if command -v xdg-open &> /dev/null; then
    echo "Opening browser..."
    xdg-open http://localhost:8080 &
elif command -v open &> /dev/null; then
    open http://localhost:8080 &
fi

echo ""
echo "Press Ctrl+C to stop all servers"
echo ""

# Cleanup on exit
trap "echo ''; echo '🛑 Stopping servers...'; kill $SERVER_PID $BRIDGE_PID $HTTP_PID 2>/dev/null; echo '✅ All stopped'; exit" INT

wait
