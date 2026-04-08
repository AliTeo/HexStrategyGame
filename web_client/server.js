const WebSocket = require('ws');
const net = require('net');
const http = require('http');
const express = require('express');
const path = require('path');

const PORT = process.env.PORT || 3000;
const TCP_HOST = process.env.TCP_HOST || 'localhost';
const TCP_PORT = process.env.TCP_PORT || 12345;

console.log('========================================');
console.log('  Hex Game All-in-One Server');
console.log('========================================');
console.log(`HTTP + WebSocket Server: http://0.0.0.0:${PORT}`);
console.log(`TCP Backend: ${TCP_HOST}:${TCP_PORT}`);
console.log('========================================\n');

// Create Express app for serving static files
const app = express();
app.use(express.static(__dirname));

// Create HTTP server
const server = http.createServer(app);

// Create WebSocket server on the same port
const wss = new WebSocket.Server({ server });

wss.on('connection', (ws, req) => {
    const clientIP = req.socket.remoteAddress;
    console.log(`[WebSocket] New connection from ${clientIP}`);
    
    // Create TCP connection to C++ server
    const tcpSocket = new net.Socket();
    let buffer = '';
    
    tcpSocket.connect(TCP_PORT, TCP_HOST, () => {
        console.log(`[TCP] Connected to game server`);
    });
    
    // TCP -> WebSocket
    tcpSocket.on('data', (data) => {
        buffer += data.toString();
        
        const messages = buffer.split('\n');
        buffer = messages.pop();
        
        messages.forEach(msg => {
            if (msg.trim()) {
                try {
                    ws.send(msg);
                    console.log(`[TCP→WS] ${msg.substring(0, 50)}...`);
                } catch (error) {
                    console.error('[TCP→WS] Error:', error.message);
                }
            }
        });
    });
    
    tcpSocket.on('error', (error) => {
        console.error(`[TCP] Error: ${error.message}`);
        ws.send(JSON.stringify({
            type: 'error',
            message: 'Connection to game server failed'
        }));
        ws.close();
    });
    
    tcpSocket.on('close', () => {
        console.log('[TCP] Connection closed');
        ws.close();
    });
    
    // WebSocket -> TCP
    ws.on('message', (message) => {
        try {
            const data = message.toString();
            console.log(`[WS→TCP] ${data.substring(0, 50)}...`);
            tcpSocket.write(data + '\n');
        } catch (error) {
            console.error('[WS→TCP] Error:', error.message);
        }
    });
    
    ws.on('close', () => {
        console.log(`[WebSocket] Client disconnected`);
        tcpSocket.end();
    });
    
    ws.on('error', (error) => {
        console.error(`[WebSocket] Error: ${error.message}`);
        tcpSocket.end();
    });
});

// Start server
server.listen(PORT, '0.0.0.0', () => {
    console.log(`✓ Server listening on port ${PORT}`);
    console.log(`\nOpen in browser: http://localhost:${PORT}`);
    console.log(`Or share via ngrok: ngrok http ${PORT}\n`);
});

// Graceful shutdown
process.on('SIGINT', () => {
    console.log('\n\nShutting down...');
    server.close(() => {
        console.log('Server closed');
        process.exit(0);
    });
});
