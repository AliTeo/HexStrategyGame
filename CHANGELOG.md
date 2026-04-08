# Changelog

All notable changes to the Hexagonal Strategy Game project.

## [1.0.1] - 2026-04-05

### Fixed
- **Message Timing Issue**: Fixed race condition where user input prompt appeared before phase transition messages
  - Added 100ms initial delay after connection to allow server messages to arrive
  - Added 50ms message processing delays in main loop
  - Double message processing before input to catch any delayed messages
  - Improved phase transition displays with clearer formatting

### Improved
- **User Experience**: 
  - Added phase-specific prompts: `[SELECT]`, `[PLACE]`, `[BATTLE]`
  - Enhanced phase transition messages with better visual separation
  - Added "Waiting for..." messages to indicate what the player should do
  - Clearer instructions at each game phase

### Documentation
- Added **QUICKSTART.md** with step-by-step gameplay guide
- Updated plan.md to reflect completion status

## [1.0.0] - 2026-04-05

### Added - Initial Release
- Complete hexagonal grid system (7x7, axial coordinates)
- 6 character classes with unique abilities
- 2-player online multiplayer via TCP
- Character selection system (3 from pool of 6)
- Strategic placement phase
- Simultaneous turn-based combat
- Skill system with 12 different abilities
- Text-based UI for prototype testing
- Authoritative server architecture
- JSON-based network protocol
- Professional project structure
- Comprehensive documentation

### Technical Implementation
- C++17 with modern features
- ASIO for async networking
- nlohmann/json for serialization
- Smart pointers throughout (memory safe)
- Build system with CMake and shell script
- ~8,000 lines of code across 30 source files

### Documentation
- README.md - Project overview
- TESTING.md - Complete testing guide
- PROJECT_SUMMARY.md - Feature documentation
- ARCHITECTURE.md - System design diagrams
- COMPLETION_STATUS.md - Project status
- build.sh - Automated build
- test_game.sh - Quick test launcher

---

## Legend
- **Added**: New features
- **Changed**: Changes to existing functionality
- **Deprecated**: Soon-to-be removed features
- **Removed**: Removed features
- **Fixed**: Bug fixes
- **Security**: Security improvements
- **Improved**: Enhancements to existing features
