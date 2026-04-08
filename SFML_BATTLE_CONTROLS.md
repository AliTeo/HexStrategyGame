# SFML Battle Phase Controls

## Overview

The SFML client now features an intuitive click-to-command system for the Battle phase. Actions are queued locally and visualized with colored arrows before being submitted.

## Controls

### Character Selection
1. **Click on a character** (your own) to select it
   - A yellow highlight appears around the selected character
   - Console shows: "Selected character: [ID]"

### Choosing Action Mode
After selecting a character:
- Press **M** - Enter MOVE mode
- Press **A** - Enter ATTACK mode

### Executing Commands

**MOVE Mode:**
- Click on any hex to queue a movement action
- A **blue arrow** appears showing the planned move
- Character is deselected after queuing

**ATTACK Mode:**
- Click on any hex to queue an attack/skill action
- A **red arrow** appears showing the attack target
- Uses the character's first damage skill (fireball, slash, etc.)
- Character is deselected after queuing

### Locking Turn
- Press **SPACEBAR** to submit all queued actions and lock your turn
- All pending actions are sent to the server
- Characters without actions automatically PASS
- Wait for opponent to lock their turn

### Visual Feedback

**Arrow Colors:**
- 🔵 **Blue** - Movement action
- 🔴 **Red** - Attack/Skill action
- 🟡 **Yellow highlight** - Currently selected character

**Important:** Arrows are only visible to you! They disappear after locking turn.

## Workflow Example

```
1. Click on your Warrior → Yellow highlight appears
2. Press 'M' → Console shows "Mode: MOVE"
3. Click destination hex → Blue arrow appears from Warrior to hex
4. Click on your Mage → Yellow highlight moves to Mage
5. Press 'A' → Console shows "Mode: ATTACK"
6. Click enemy position → Red arrow appears from Mage to target
7. (Your Healer is not assigned - will automatically PASS)
8. Press SPACEBAR → All actions submitted, turn locked
```

## Notes

- You can only assign **one action per character** per turn
- Clicking a character without pressing M or A just selects it
- You don't need to assign actions to all characters
- Characters without actions will automatically PASS when you lock
- Actions are NOT visible to your opponent until after resolution
- Press **ESC** to quit the game

## Comparison with Text Client

| Action | Text Client | SFML Client |
|--------|-------------|-------------|
| Select character | N/A | Click character |
| Move | `move p0_c0 -1 0` | Select + M + Click hex |
| Attack | `skill p0_c1 fireball 2 -1` | Select + A + Click hex |
| Pass | `pass p0_c2` | Just don't assign action |
| Lock | `ready` | SPACEBAR |

## Future Enhancements

- Right-click to deselect/cancel
- Skill selection menu (multiple skills)
- Undo last action
- Visual range indicators
- Health/stat tooltips on hover
