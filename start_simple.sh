#!/bin/bash
# Simplified all-in-one server for ngrok/dorms

echo "=========================================="
echo "  Starting Hex Game (Ngrok Ready)"
echo "=========================================="
echo ""

# Check prerequisites
if ! command -v node &> /dev/null; then
    echo "❌ Node.js not found! Install: sudo pacman -S nodejs npm"
    exit 1
fi

if [ ! -f "./bin/HexGameServer" ]; then
    echo "❌ Game server not built! Run: ./build.sh"
    exit 1
fi

if [ ! -d "./web_client/node_modules" ]; then
    echo "📦 Installing dependencies..."
    cd web_client && npm install && cd ..
fi

# Stop any existing processes
pkill -f HexGameServer 2>/dev/null
pkill -f "node.*server.js" 2>/dev/null
sleep 1

echo "🚀 Starting game server..."
./bin/HexGameServer > /tmp/hexgame_server.log 2>&1 &
SERVER_PID=$!
echo "✅ Game server started (PID: $SERVER_PID)"
sleep 2

echo "🌐 Starting web server (HTTP + WebSocket)..."
cd web_client
node server.js &
WEB_PID=$!
cd ..
echo "✅ Web server started on port 3000 (PID: $WEB_PID)"
sleep 2

echo ""
echo "=========================================="
echo "  ✅ ALL RUNNING!"
echo "=========================================="
echo ""
echo "📍 Local testing:"
echo "   http://localhost:3000"
echo ""
echo "🌍 For remote friend (use ngrok):"
echo "   Run in another terminal:"
echo "   ngrok http 3000"
echo ""
echo "   Then send friend the ngrok URL!"
echo ""
echo "Press Ctrl+C to stop"
echo ""

trap "echo ''; echo '🛑 Stopping...'; kill $SERVER_PID $WEB_PID 2>/dev/null; echo '✅ Done'; exit" INT

wait
