# Quick Start Guide

## Starting the Game

### Step 1: Start the Server
```bash
cd HexGame
./bin/HexGameServer
```

You'll see:
```
========================================
  Hexagonal Strategy Game Server
========================================
Port: 12345
Waiting for players...
```

### Step 2: Start First Client (Player 0)
```bash
cd HexGame
./bin/HexGameClient
```

You'll see:
```
  _   _              ____  _        _                     
 | | | | _____  __  / ___|| |_ _ __| |_                  
 | |_| |/ _ \ \/ /  \___ \|  _| '__| __|               
 |  _  |  __/>  <    ___) | |_| |  | |_                  
 |_| |_|\___/_/\_\  |____/ \__|_|   \__|              

         [PROTOTYPE - TEXT MODE]

Welcome to Hexagonal Strategy Game!
====================================

✓ Connected as Player 0
Waiting for opponent...
```

### Step 3: Start Second Client (Player 1)
In another terminal:
```bash
cd HexGame
./bin/HexGameClient
```

Both clients will now see:
```
✓ Player 1 joined
✓ Game starting! Both players connected.
Waiting for game state...

==========================================
     CHARACTER SELECTION PHASE
==========================================
```

## Phase 1: Character Selection

Both players will see the character list and can select 3:

```
AVAILABLE CHARACTER CLASSES:
-----------------------------
0. Warrior  - Melee fighter (HP:120, ATK:15, DEF:8)
1. Mage     - Ranged damage (HP:80, ATK:20, DEF:3)
2. Healer   - Support healer (HP:90, ATK:8, DEF:5)
3. Rogue    - High mobility (HP:85, ATK:18, DEF:4)
4. Tank     - Heavy defense (HP:150, ATK:10, DEF:12)
5. Summoner - Hex control (HP:75, ATK:12, DEF:4)

Select 3 characters for your team.

Waiting for your selection...
[SELECT] > 
```

**Commands for this phase:**
```
select 0    # Select Warrior
select 1    # Select Mage  
select 2    # Select Healer
```

After both players select 3 characters, the game automatically advances.

## Phase 2: Character Placement

```
==========================================
     CHARACTER PLACEMENT PHASE
==========================================
Place your 3 characters on your side of the grid
You are Player 0: place on LEFT side (q < 0)
Use 'place <class_id> <q> <r>' to place a character

[PLACE] >
```

**Player 0 (left side, q < 0):**
```
place 0 -2 0     # Place Warrior at (-2, 0)
place 1 -2 1     # Place Mage at (-2, 1)
place 2 -2 -1    # Place Healer at (-2, -1)
```

**Player 1 (right side, q > 0):**
```
place 0 2 0      # Place Warrior at (2, 0)
place 1 2 -1     # Place Mage at (2, -1)
place 2 2 1      # Place Healer at (2, 1)
```

The hex grid will show your placements in real-time.

## Phase 3: Battle

```
==========================================
         ⚔️  BATTLE BEGINS! ⚔️
==========================================
Submit actions for your characters, then type 'ready'

Enter your commands...
[BATTLE] >
```

### Available Commands:

**View the battlefield:**
```
view
```

**Move a character:**
```
move p0_c0 -1 0           # Move your first character to (-1, 0)
```

**Use a skill:**
```
skill p0_c1 fireball 2 -1  # Your mage uses fireball on target hex
```

**Skip a character's turn:**
```
pass p0_c2                 # Your healer does nothing
```

**Lock in your actions:**
```
ready                      # Commit your turn
```

### Example Turn:

**Player 0:**
```
[BATTLE] > view                          # Check the board
[BATTLE] > move p0_c0 -1 0              # Warrior moves forward
[BATTLE] > skill p0_c1 fireball 2 -1    # Mage attacks enemy
[BATTLE] > pass p0_c2                   # Healer waits
[BATTLE] > ready                        # Lock in
Turn locked in. Waiting for opponent...
```

**Player 1:**
```
[BATTLE] > move p1_c0 1 0               # Warrior advances
[BATTLE] > skill p1_c2 heal 2 0         # Healer heals warrior
[BATTLE] > skill p1_c1 fireball -2 1    # Mage attacks
[BATTLE] > ready                        # Lock in
Turn locked in. Waiting for opponent...
```

### Turn Resolution:

When both players type `ready`, you'll see:
```
=== Turn 1 Results ===
  Warrior moves to (-1,0)
  Mage uses Fireball dealing 30 damage to Mage
  Warrior moves to (1,0)
  Mage uses Fireball dealing 30 damage to Mage
  Healer uses Heal healing Warrior for 35
  ⚠ Mage has been defeated!
======================

New turn begins!
```

## Character IDs

Your characters are identified as:
- **Player 0**: `p0_c0`, `p0_c1`, `p0_c2`
- **Player 1**: `p1_c0`, `p1_c1`, `p1_c2`

The order matches your selection order (0=first selected, 1=second, 2=third).

## Skill IDs

**Warrior:**
- `slash` - Melee attack (range 1)
- `charge` - Push attack (range 2)

**Mage:**
- `fireball` - Ranged damage (range 4)
- `ice_wall` - Create terrain (range 3)

**Healer:**
- `heal` - Restore HP (range 3)
- `shield` - Protect ally (range 2)

**Rogue:**
- `backstab` - High damage (range 1)
- `dash` - Extra movement (range 0, self)

**Tank:**
- `taunt` - Pull enemies (range 3)
- `fortify` - Increase defense (range 0, self)

**Summoner:**
- `summon` - Create minion (range 2)
- `control` - Hex control (range 3)

## Other Commands

At any time:
```
help    # Show command list
view    # Display current game state
quit    # Disconnect from server
```

## Tips

1. **Use `view` often** to see the current board state
2. **Plan your moves** - you can submit multiple actions before typing `ready`
3. **Character placement matters** - position for ranged vs melee
4. **Coordinate skills** - healers can support damaged allies
5. **Watch your timing** - wait for phase messages before typing commands

## Troubleshooting

**"Invalid placement" error:**
- Make sure you're placing on your side (Player 0: q < 0, Player 1: q > 0)
- Check that the hex isn't already occupied

**"Skill out of range" error:**
- Check the skill's range with `view`
- Count hex distance from your character to target

**Commands not working:**
- Wait for the phase-specific prompt ([SELECT], [PLACE], [BATTLE])
- Type `help` to see available commands

**Connection lost:**
- Make sure server is still running
- Restart client and reconnect

## Winning the Game

The game ends when all of one player's characters are defeated!

```
==========================================
         GAME OVER
==========================================
🎉 YOU WIN! 🎉
```

Good luck, Commander! 🎮
