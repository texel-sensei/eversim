# eversim

eversim is a 2D game engine written in modern C++. Its goal is to support
games which depend on different systems to simulate a living and breathing world
and to create emergent gameplay. See the videos of GMTK about [immersive
sims](https://www.youtube.com/watch?v=kbyTOAlhRHk) and
[systemic games](https://www.youtube.com/watch?v=SnpAAX9CkIc) for reference.

It runs on Windows and support for Linux is in work.

## Available modules

The engine is divided into multiple modules, each performing one part of
simulating and running a game.

Right now, the following modules are implemented:

- Basic rendering
- Physics
- Input handling

## Compiling

To build eversim, you need a modern C++ compiler and CMake.
Additionally the following libraries are required:

- OpenGL
- SDL2
- GLEW
- GLM
