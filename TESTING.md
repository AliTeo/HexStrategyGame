# Testing Guide

## Quick Start

### 1. Build the Project

```bash
cd HexGame
./build.sh
```

### 2. Start the Server

In terminal 1:
```bash
./bin/HexGameServer
```

You should see:
```
========================================
  Hexagonal Strategy Game Server
========================================
Port: 12345
Waiting for players...
```

### 3. Start Two Clients

In terminal 2 (Player 0):
```bash
./bin/HexGameClient
```

In terminal 3 (Player 1):
```bash
./bin/HexGameClient
```

## Complete Game Test Walkthrough

### Phase 1: Character Selection

**Both players** must select 3 characters from the pool of 6.

Player 0 terminal:
```
> select 0
> select 1
> select 2
```

Player 1 terminal:
```
> select 0
> select 1
> select 2
```

Once both players have selected 3 characters, the game automatically advances to Placement phase.

### Phase 2: Character Placement

**Player 0** places on the LEFT side (q < 0):
```
> place 0 -2 0   # Warrior at (-2, 0)
> place 1 -2 1   # Mage at (-2, 1)
> place 2 -2 -1  # Healer at (-2, -1)
```

**Player 1** places on the RIGHT side (q > 0):
```
> place 0 2 0    # Warrior at (2, 0)
> place 1 2 -1   # Mage at (2, -1)
> place 2 2 1    # Healer at (2, 1)
```

Once both players have placed all 3 characters, the game automatically starts!

### Phase 3: Battle

View the current state:
```
> view
```

#### Turn 1 - Player 0 Example:

```
> move p0_c0 -1 0           # Move Warrior forward to (-1, 0)
> skill p0_c1 fireball 2 -1 # Mage uses fireball on enemy Mage at (2, -1)
> pass p0_c2                # Healer does nothing this turn
> ready                     # Lock in your actions
```

#### Turn 1 - Player 1 Example:

```
> move p1_c0 1 0            # Move Warrior forward to (1, 0)
> skill p1_c1 fireball -2 1 # Mage uses fireball on enemy Mage at (-2, 1)
> pass p1_c2                # Healer does nothing
> ready                     # Lock in your actions
```

**When both players type `ready`**, the turn resolves and you'll see:
```
=== Turn 1 Results ===
  Warrior moves to (-1,0)
  Mage uses Fireball dealing 30 damage to Mage
  Warrior moves to (1,0)
  Mage uses Fireball dealing 30 damage to Mage
  ⚠ Mage has been defeated!
  ⚠ Mage has been defeated!
======================
```

### Continue Playing

Repeat the battle phase until one player's characters are all defeated.

## Character Classes & Skills

### 0. Warrior (HP: 120, ATK: 15, DEF: 8, MOV: 3)
- **slash**: Melee damage (range 1, power 25)
- **charge**: Push enemy (range 2, power 15, area 1)

### 1. Mage (HP: 80, ATK: 20, DEF: 3, MOV: 2)
- **fireball**: Ranged damage (range 4, power 30)
- **ice_wall**: Alter terrain (range 3)

### 2. Healer (HP: 90, ATK: 8, DEF: 5, MOV: 3)
- **heal**: Restore ally HP (range 3, power 35)
- **shield**: Protect ally (range 2, power 20)

### 3. Rogue (HP: 85, ATK: 18, DEF: 4, MOV: 5)
- **backstab**: High damage (range 1, power 40)
- **dash**: Extra movement (range 0, power 3)

### 4. Tank (HP: 150, ATK: 10, DEF: 12, MOV: 2)
- **taunt**: Pull enemies (range 3, power 10, area 2)
- **fortify**: Increase defense (range 0, power 15)

### 5. Summoner (HP: 75, ATK: 12, DEF: 4, MOV: 2)
- **summon**: Create minion (range 2, power 20)
- **control**: Hex control (range 3, area 1)

## Common Commands

- `view` - Display current game state and hex grid
- `help` - Show all available commands
- `quit` - Disconnect from game

## Hex Coordinate System

The game uses axial coordinates (q, r):
- q increases to the right (East)
- r increases down-right (Southeast)
- Valid coordinates: -3 ≤ q, r ≤ 3 (7x7 grid)

Example grid positions:
```
      (-3,-3)(-2,-3)(-1,-3)(0,-3)
    (-3,-2)(-2,-2)(-1,-2)(0,-2)(1,-2)
  (-3,-1)(-2,-1)(-1,-1)(0,-1)(1,-1)(2,-1)
(-3,0)(-2,0)(-1,0)(0,0)(1,0)(2,0)(3,0)
  (-2,1)(-1,1)(0,1)(1,1)(2,1)(3,1)
    (-1,2)(0,2)(1,2)(2,2)(3,2)
      (0,3)(1,3)(2,3)(3,3)
```

Player 0 side: q < 0 (left)
Player 1 side: q > 0 (right)
Neutral zone: q = 0 (center column)

## Troubleshooting

**Server won't start:**
- Check if port 12345 is already in use
- Try: `./bin/HexGameServer 12346` to use different port
- Then: `./bin/HexGameClient localhost 12346`

**Client can't connect:**
- Make sure server is running first
- Check firewall settings

**Commands not working:**
- Use `help` to see command format
- Check character IDs with `view`
- Make sure you're in the correct game phase

## Advanced Testing

### Different Character Compositions

Test 1 - All melee:
```
select 0 (Warrior)
select 3 (Rogue)
select 4 (Tank)
```

Test 2 - Balanced:
```
select 0 (Warrior)
select 1 (Mage)
select 2 (Healer)
```

Test 3 - Ranged/Support:
```
select 1 (Mage)
select 2 (Healer)
select 5 (Summoner)
```

### Test Scenarios

1. **Range Test**: Use Mage fireball at maximum range (4 hexes)
2. **Healing Test**: Damage a character, then heal it
3. **Movement Test**: Move characters around the battlefield
4. **Area Skills**: Test skills with area effects

## Next Steps

After testing the text prototype:
1. Add more skill combinations
2. Implement terrain effects
3. Add SFML graphics layer
4. Implement reconnection logic
5. Add game replay/history
