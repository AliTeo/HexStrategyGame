#!/usr/bin/env node

/**
 * WebSocket Bridge for Hex Game
 * Converts WebSocket connections to TCP for the C++ server
 */

const WebSocket = require('ws');
const net = require('net');

const WS_PORT = process.env.WS_PORT || 9000;
const TCP_HOST = process.env.TCP_HOST || 'localhost';
const TCP_PORT = process.env.TCP_PORT || 12345;

console.log('========================================');
console.log('  Hex Game WebSocket Bridge');
console.log('========================================');
console.log(`WebSocket Server: ws://0.0.0.0:${WS_PORT}`);
console.log(`TCP Backend: ${TCP_HOST}:${TCP_PORT}`);
console.log('========================================\n');

// Create WebSocket server
const wss = new WebSocket.Server({ port: WS_PORT });

wss.on('listening', () => {
    console.log(`✓ WebSocket server listening on port ${WS_PORT}`);
    console.log(`\nYour friend can now connect via browser:`);
    console.log(`  http://YOUR_IP:${WS_PORT}\n`);
});

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
        
        // Split by delimiter (assuming '\n' or custom delimiter)
        const messages = buffer.split('\n');
        buffer = messages.pop(); // Keep incomplete message in buffer
        
        messages.forEach(msg => {
            if (msg.trim()) {
                try {
                    // Forward JSON message to WebSocket client
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
            
            // Forward to TCP server (add delimiter if needed)
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

wss.on('error', (error) => {
    console.error(`[Server] Error: ${error.message}`);
});

// Handle graceful shutdown
process.on('SIGINT', () => {
    console.log('\n\nShutting down...');
    wss.close(() => {
        console.log('WebSocket server closed');
        process.exit(0);
    });
});
