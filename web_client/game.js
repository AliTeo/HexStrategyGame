// WebSocket Game Client
class HexGameClient {
    constructor() {
        this.ws = null;
        this.serverAddress = '';
        this.playerId = null;
        this.gameState = null;
        this.selectedCharacters = [];
        this.canvas = null;
        this.ctx = null;
        
        this.initializeUI();
    }
    
    initializeUI() {
        // Connect button
        document.getElementById('connect-btn').addEventListener('click', () => {
            this.connect();
        });
        
        // Clear log button
        document.getElementById('clear-log').addEventListener('click', () => {
            document.getElementById('log-content').innerHTML = '';
        });
        
        // Ready button
        document.getElementById('ready-btn').addEventListener('click', () => {
            this.sendReady();
        });
        
        // Canvas setup
        this.canvas = document.getElementById('hex-canvas');
        this.ctx = this.canvas.getContext('2d');
        this.resizeCanvas();
        window.addEventListener('resize', () => this.resizeCanvas());
    }
    
    resizeCanvas() {
        const container = this.canvas.parentElement;
        this.canvas.width = container.clientWidth;
        this.canvas.height = container.clientHeight;
        if (this.gameState) {
            this.draw();
        }
    }
    
    connect() {
        const btn = document.getElementById('connect-btn');
        const status = document.getElementById('connection-status');
        
        // WebSocket connection - use same host as webpage
        const protocol = window.location.protocol === 'https:' ? 'wss:' : 'ws:';
        const wsUrl = `${protocol}//${window.location.host}`;
        
        this.serverAddress = window.location.host;
        
        console.log('Connecting to:', wsUrl);
        
        btn.disabled = true;
        status.textContent = 'Connecting...';
        status.className = 'status-message info';
        
        try {
            this.ws = new WebSocket(wsUrl);
            
            this.ws.onopen = () => {
                this.log('Connected to server', 'success');
                status.textContent = 'Connected!';
                status.className = 'status-message success';
                
                setTimeout(() => {
                    this.showGameScreen();
                    // Automatically show character selection
                    this.showCharacterSelection();
                    document.getElementById('game-phase').textContent = 'Character Selection';
                }, 500);
            };
            
            this.ws.onmessage = (event) => {
                this.handleMessage(event.data);
            };
            
            this.ws.onerror = (error) => {
                this.log('Connection error', 'error');
                status.textContent = 'Connection failed. Make sure server is running.';
                status.className = 'status-message error';
                btn.disabled = false;
            };
            
            this.ws.onclose = () => {
                this.log('Disconnected from server', 'error');
                document.getElementById('connection-status-game').classList.add('disconnected');
                btn.disabled = false;
            };
            
        } catch (error) {
            status.textContent = 'Invalid server address';
            status.className = 'status-message error';
            btn.disabled = false;
        }
    }
    
    handleMessage(data) {
        try {
            const message = JSON.parse(data);
            this.log(`← ${message.type}`, 'info');
            
            switch (message.type) {
                case 'player_id':
                    this.playerId = message.player_id;
                    document.getElementById('player-id').textContent = `Player ${this.playerId}`;
                    break;
                    
                case 'game_state':
                    this.gameState = message;
                    this.updateGameState();
                    break;
                    
                case 'phase_change':
                    this.handlePhaseChange(message.phase);
                    break;
                    
                case 'turn_result':
                    this.handleTurnResult(message);
                    break;
                    
                case 'error':
                    this.log(`Error: ${message.message}`, 'error');
                    break;
                    
                default:
                    console.log('Unknown message:', message);
            }
        } catch (error) {
            console.error('Failed to parse message:', error);
        }
    }
    
    handlePhaseChange(phase) {
        document.getElementById('game-phase').textContent = phase;
        
        switch (phase) {
            case 'selection':
                this.showCharacterSelection();
                break;
            case 'placement':
                this.showPlacement();
                break;
            case 'battle':
                this.showBattle();
                break;
        }
    }
    
    showCharacterSelection() {
        this.log('Character selection phase started', 'info');
        
        // Make sure character selection panel is visible
        const selectionPanel = document.getElementById('character-selection');
        selectionPanel.classList.remove('hidden');
        
        const characters = [
            { id: 0, name: 'Warrior', hp: 120, atk: 15, def: 8, mov: 3 },
            { id: 1, name: 'Mage', hp: 80, atk: 20, def: 3, mov: 2 },
            { id: 2, name: 'Healer', hp: 90, atk: 8, def: 5, mov: 3 },
            { id: 3, name: 'Rogue', hp: 85, atk: 18, def: 4, mov: 5 },
            { id: 4, name: 'Tank', hp: 150, atk: 10, def: 12, mov: 2 },
            { id: 5, name: 'Summoner', hp: 75, atk: 12, def: 4, mov: 2 }
        ];
        
        const list = document.getElementById('character-list');
        list.innerHTML = '';
        
        characters.forEach(char => {
            const card = document.createElement('div');
            card.className = 'character-card';
            card.innerHTML = `
                <div class="character-name">${char.name}</div>
                <div class="character-stats">
                    <span class="stat">HP: ${char.hp}</span>
                    <span class="stat">ATK: ${char.atk}</span>
                    <span class="stat">DEF: ${char.def}</span>
                    <span class="stat">MOV: ${char.mov}</span>
                </div>
            `;
            
            card.addEventListener('click', () => {
                if (this.selectedCharacters.length < 3 && !this.selectedCharacters.includes(char.id)) {
                    this.selectedCharacters.push(char.id);
                    card.classList.add('selected');
                    
                    // Send to server
                    this.send({ 
                        type: 'SELECT_CHARACTER', 
                        character_id: char.id 
                    });
                    
                    this.log(`Selected ${char.name} (${this.selectedCharacters.length}/3)`, 'success');
                    
                    // Update selected display
                    this.updateSelectedDisplay();
                    
                    if (this.selectedCharacters.length === 3) {
                        this.log('All characters selected! Waiting for opponent...', 'success');
                    }
                }
            });
            
            list.appendChild(card);
        });
    }
    
    updateSelectedDisplay() {
        const container = document.getElementById('selected-characters');
        container.innerHTML = `<p>Selected: ${this.selectedCharacters.length}/3</p>`;
    }
    
    showPlacement() {
        this.log('Placement phase started', 'info');
        // Canvas becomes interactive for placement
        this.canvas.addEventListener('click', (e) => this.handleCanvasClick(e));
    }
    
    showBattle() {
        this.log('Battle phase started!', 'success');
        document.getElementById('battle-actions').classList.remove('hidden');
    }
    
    handleCanvasClick(event) {
        const rect = this.canvas.getBoundingClientRect();
        const x = event.clientX - rect.left;
        const y = event.clientY - rect.top;
        
        // Convert screen coords to hex coords (simplified)
        // This needs proper hex grid calculation
        const hexCoords = this.screenToHex(x, y);
        
        this.log(`Clicked hex: ${hexCoords.q}, ${hexCoords.r}`, 'info');
        
        // Send placement or action based on phase
        if (this.gameState && this.gameState.phase === 'placement') {
            this.send({
                type: 'place',
                character_id: 0, // TODO: select which character to place
                q: hexCoords.q,
                r: hexCoords.r
            });
        }
    }
    
    screenToHex(x, y) {
        // Simple conversion - needs proper hex math
        const size = 30;
        const centerX = this.canvas.width / 2;
        const centerY = this.canvas.height / 2;
        
        const q = Math.round((x - centerX) / (size * 1.5));
        const r = Math.round((y - centerY) / (size * Math.sqrt(3)));
        
        return { q, r };
    }
    
    draw() {
        if (!this.ctx) return;
        
        // Clear canvas
        this.ctx.fillStyle = '#1a1a2e';
        this.ctx.fillRect(0, 0, this.canvas.width, this.canvas.height);
        
        // Draw hex grid (7x7)
        const size = 30;
        const centerX = this.canvas.width / 2;
        const centerY = this.canvas.height / 2;
        
        for (let q = -3; q <= 3; q++) {
            for (let r = -3; r <= 3; r++) {
                if (Math.abs(q + r) <= 3) {
                    this.drawHex(centerX, centerY, q, r, size);
                }
            }
        }
        
        // Draw characters if game state exists
        if (this.gameState && this.gameState.characters) {
            // TODO: Draw characters on grid
        }
    }
    
    drawHex(centerX, centerY, q, r, size) {
        const x = centerX + size * 1.5 * q;
        const y = centerY + size * Math.sqrt(3) * (r + q / 2);
        
        this.ctx.beginPath();
        for (let i = 0; i < 6; i++) {
            const angle = Math.PI / 3 * i;
            const hx = x + size * Math.cos(angle);
            const hy = y + size * Math.sin(angle);
            if (i === 0) {
                this.ctx.moveTo(hx, hy);
            } else {
                this.ctx.lineTo(hx, hy);
            }
        }
        this.ctx.closePath();
        
        this.ctx.strokeStyle = '#4CAF50';
        this.ctx.lineWidth = 1;
        this.ctx.stroke();
        
        this.ctx.fillStyle = 'rgba(76, 175, 80, 0.1)';
        this.ctx.fill();
        
        // Draw coords
        this.ctx.fillStyle = '#888';
        this.ctx.font = '10px monospace';
        this.ctx.textAlign = 'center';
        this.ctx.fillText(`${q},${r}`, x, y);
    }
    
    updateGameState() {
        // Redraw canvas with new game state
        this.draw();
    }
    
    handleTurnResult(result) {
        this.log('Turn resolved:', 'success');
        if (result.actions) {
            result.actions.forEach(action => {
                this.log(`  ${action}`, 'info');
            });
        }
    }
    
    send(data) {
        if (this.ws && this.ws.readyState === WebSocket.OPEN) {
            this.ws.send(JSON.stringify(data));
            this.log(`→ ${data.type}`, 'info');
        }
    }
    
    sendReady() {
        this.send({ type: 'ready' });
        document.getElementById('ready-btn').disabled = true;
    }
    
    showGameScreen() {
        document.getElementById('connect-screen').classList.remove('active');
        document.getElementById('game-screen').classList.add('active');
        document.getElementById('server-address').textContent = this.serverAddress;
        
        this.resizeCanvas();
        this.draw();
    }
    
    log(message, type = 'info') {
        const logContent = document.getElementById('log-content');
        const entry = document.createElement('div');
        entry.className = `log-entry ${type}`;
        entry.textContent = `[${new Date().toLocaleTimeString()}] ${message}`;
        logContent.appendChild(entry);
        logContent.scrollTop = logContent.scrollHeight;
    }
}

// Initialize client when page loads
window.addEventListener('DOMContentLoaded', () => {
    new HexGameClient();
});
