# War1

A remake of Warcraft 1 written in C

This is a remake of the Warcraft: Orcs & Humans videogame created and published by Blizzard Entertainment in 1994. It pretends to be a complete implementation of the game using only the assets (*.WAR files) from the original game.

Unfortunately I'm not allowed to distribute the DATA.WAR file due to copyright issues, but if you have that file from the original game you should be able to compile and run the project.

## How to build

1. Clone the repository `git clone https://github.com/acoto87/war1`

### Windows
2. Download and install [MinGW](http://www.mingw.org/) (or [MinGW-w64](http://mingw-w64.org) for 64 bits builds).
3. Get the DATA.WAR file and place it in the `assets` folder.
4. Run `build-gcc-win32.bat` (or `build-gcc-win64.bat` for 64 bits builds).

### Raspberry PI
2. Install `gcc` (usually comes with the `build-essential` package).
3. Install `libx11-dev` package.
4. Run `build-gcc-linux.sh`.

## Libraries used

* [GLFW](https://www.glfw.org/): An Open Source, multi-platform library for OpenGL, OpenGL ES and Vulkan development on the desktop. It provides a simple API for creating windows, contexts and surfaces, receiving input and events.
* [nanovg](https://github.com/memononen/nanovg): NanoVG is small antialiased vector graphics rendering library for OpenGL. It has lean API modeled after HTML5 canvas API. It is aimed to be a practical and fun toolset for building scalable user interfaces and visualizations.
* [Miniaudio](https://github.com/dr-soft/miniaudio) Audio playback and capture library.
* [TinySoundFont](https://github.com/schellingb/TinySoundFont) SoundFont2 synthesizer.
* [shl](https://github.com/acoto87/shl): Single header libraries with commonly used data structures.
* [stb](https://github.com/nothings/stb) Single-file public domain libraries for C/C++

Here is my [TODO](https://github.com/acoto87/war1/blob/master/todo.md) list.

## Screnshots

![](https://github.com/acoto87/war1/blob/master/pics/GIF14.gif)
![](https://github.com/acoto87/war1/blob/master/pics/GIF30.gif)
![](https://github.com/acoto87/war1/blob/master/pics/GIF32.gif)