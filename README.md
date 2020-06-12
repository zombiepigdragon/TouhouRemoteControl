# Touhou Remote Control
A plugin for [thcrap](https://github.com/thpatch/thcrap) that allows for using any compliant frontend to manipulate Touhou games.

### Current Status: One variable for Th12. Needs work.

## Installation
Once releases are available, you can do the following:
- Place `remote.dll` into your `thcrap/bin` directory
- Copy the `zombie_pigdragon` folder into your `thcrap/repos` directory
- Add the plugin to a run configuration of your choice.
- Run the game and a compatible frontend.

## Compiling
As there is still quite a lot to do before that, do this instead:
- Setup a mingw compiler (Sorry, but I'm doing this on Linux. I couldn't run Visual Studio if I wanted to.)
- Adjust the Makefile for your machine (This shouldn't actually be needed, as long as `i686-w64-mingw32-gcc` is available)
- Run the command `make` in this directory.
- Follow the installation instructions (Rename the `thcrap_repo` directory to `zombie_pigdragon`)

## Usage
I'll make this once there's a frontend. For now, just look at port 8646 with the game running (try `nc localhost:8646`.)
It should respond to any message with the current score. I could have made it read/write, but it's implemented in the wrong place right now. Just know it's available.