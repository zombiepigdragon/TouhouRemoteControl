# Touhou Remote Control
A plugin for [thcrap](https://github.com/thpatch/thcrap) that simplifies the production of programs that interface with Team Shanghai Alice Touhou games.

> Status: Builds are not stable, most memory values are not known. USE AT YOUR OWN PERIL.

## Installation
> Developer's note: releases will be produced once at least one game is fully functional.
1) Download the most recent release from [Releases](/releases/latest/download/touhouremotecontrol.zip)
2) Place the contents of the `thcrap` directory into your `thcrap` install. (There should be some merging directories.)
3) Add the plugin to a run configuration of your choice.
4) Run the game and any compatible program. *(Developer's note: Once releases are available, a sample program will be included)*.

## Compiling
As there is still quite a lot to do before that, do this instead:
1) Setup a mingw compiler (Sorry, but I'm doing this on Linux. I couldn't run Visual Studio if I wanted to.)
2) Adjust the Makefile for your machine (This shouldn't actually be needed, as long as `i686-w64-mingw32-gcc` is available)
3) Run the command `make` in this directory.
4) Place `remote.dll` into your `thcrap/bin` directory
5) Copy the `thcrap_repo` folder as `zombie_pigdragon` into your `thcrap/repos` directory
6) Add the plugin to a run configuration of your choice.

## Usage
### Standard Users:
Follow the [installation instructions](#installation) and run your program. It's that simple!

### Application developers:
This plugin provides a json interface directly over network sockets. Once more finalizing has taken place, a description of the format will be added to the repository.

### Mod developers:
If you wish to contribute or want technical information on how this works, see the [developer information](DEVELOPER.md).