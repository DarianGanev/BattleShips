# Battle Ships Game

A cross-platform implementation of the classic Battleships game in C.

## Features

- Two-player multiplayer mode
- Cross-platform compatibility (Windows, macOS, Linux)
- Manual ship placement with validation
- Attack system with hit/miss feedback
- Ship sinking detection
- Clean console interface

## Compilation

### Using gcc directly:

```bash
gcc -Wall -Wextra -std=c99 main.c game.c gameplay.c -o battleships
```

### Using Makefile:

```bash
make
```

## How to Play

1. Run the executable
2. Choose two-player mode
3. Enter player names
4. Each player places 10 ships:
   - 4 small ships (length 2)
   - 3 medium ships (length 3)
   - 2 large ships (length 4)
   - 1 cruiser (length 6)
5. Take turns attacking opponent's board
6. First player to sink all enemy ships wins!

## Ship Placement

Ships are placed by specifying:

- Starting position (e.g., A1, B5, J10)
- Direction (0 for horizontal, 1 for vertical)

Ships cannot touch each other (no adjacent sides or corners).

## Game Controls

During ship setup:

1. Place next ship
2. Edit ship position (not yet implemented)
3. View current board
4. Load from file (not yet implemented)

During battle:

1. View attack board
2. Make attack with coordinates
3. Make relative attack (not yet implemented)

## Project Structure

- `main.c` - Entry point and main menu
- `game.c` - Core game logic and board operations
- `gameplay.c` - Game flow and user interaction
- `battleships.h` - Header file with all declarations
- `Makefile` - Cross-platform build configuration

## Future Features

- Single-player mode with AI
- File-based ship configuration loading
- Game saving and loading with encryption
- Relative attack system
- Ship position editing

## Requirements

- C99 compatible compiler (gcc recommended)
- Standard C library
- Cross-platform: Windows, macOS, Linux
