# A* Pathfinding Visualization

Visualizes the A* pathfinding algorithm using SDL2 and OpenGL.

## Features

- Set start, end, and barriers on a grid.
- Visualize the A* algorithm in real-time.

## Requirements

- SDL2
- GLEW
- OpenGL

## Installation

```bash
# Ubuntu/Debian
sudo apt-get install libsdl2-dev libglew-dev

# Fedora
sudo dnf install SDL2-devel glew-devel

# Arch Linux
sudo pacman -S sdl2 glew
```

## Compilation 
```g++ main.cpp -o a_star -lSDL2 -lGL -lGLEW -lGLU```

## Usage 
```./a_star ```

## Controls
Left Click: Set start, end, and barriers
Right Click: Reset cells
Spacebar: Run algorithm
C Key: Clear grid
