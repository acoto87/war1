# War1

A remake of Warcraft: Orcs & Humans written in C.

This is a remake of the Warcraft: Orcs & Humans game created and published by Blizzard Entertainment in 1994. It pretends to be a complete implementation of the game using only the assets (*.WAR files) from the original game.

War1 is not an official Blizzard Entertainment product. It intent to be an enhancement of the original product with modern RTS features. You will need a copy of the original Warcraft: Orcs & Humans MS-DOS (or shareware) version be able to play War1. You can get it here Warcraft: Orcs and Humans. Warcraft and all graphics you see in the game are a registered trademark of Blizzard Entertainment.

Current features (this list is not complete):

* Path finding and collisions detection
* Unit movement with animations
* Unit training
* Building construction
* Resources gathering by peasants and peons
* Features specification for maps (this is specify if the player can train, build or research certain units, buildings or spells)
* Win/lose condition checking
* Fog of war
* Basic behavior for enemies (it will attack your units if you get near enough, it will chase your units)
* Spells
* Typing commands for cheats and other stuff that could be activated by it in the engine

Still to develop (this list is not complete):

* AI (I'm planning to do a basic one first and build from that more complex ones)
* Cinematics (I've the code for reading FLIC files just need to integrate it into the engine)
* Save/Load

You can check videos of gameplay and features as they are built [here](https://www.youtube.com/playlist?list=PLgN8fwyHpZaY42SjuwAQ4MPJ1rD9w1Kym).

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

## How to run

Since I'm not able to distribute the DATA.WAR file, which contains the original assets, there is a little process to get the game working properly (this applies if you don't own a copy of the game that you can purchase here [Warcraft: Orcs and Humans](https://www.gog.com/game/warcraft_orcs_and_humans)).

If you own an original copy of the game, or just bought the game on GOG, find the file DATA.WAR and just copy and paste it into the War 1 folder, and execute war1.exe.

Or just start the game and there is an option within the game to download the file.

If you don't own the original game and want to try it with the demo version, just download the demo version of [DATA.WAR](https://archive.org/download/WarcraftOrcsHumansDemo/WCRFT.ZIP/DEMODATA%2FDATA.WAR). Or just start the game and there is an option within the game to download the file.

If you want to check were that file comes from, it's from here: https://archive.org/details/WarcraftOrcsHumansDemo. If you click SEE ALL in the DOWNLOADS section you will see a WCRFT.ZIP (View Contents) entry, just click on View Contents and there is a line with the DATA.WAR file of the demo version.

## Libraries used

* [GLFW](https://www.glfw.org/): An Open Source, multi-platform library for OpenGL, OpenGL ES and Vulkan development on the desktop. It provides a simple API for creating windows, contexts and surfaces, receiving input and events.
* [nanovg](https://github.com/memononen/nanovg): NanoVG is small antialiased vector graphics rendering library for OpenGL. It has lean API modeled after HTML5 canvas API. It is aimed to be a practical and fun toolset for building scalable user interfaces and visualizations.
* [Miniaudio](https://github.com/dr-soft/miniaudio) Audio playback and capture library.
* [TinySoundFont](https://github.com/schellingb/TinySoundFont) SoundFont2 synthesizer.
* [shl](https://github.com/acoto87/shl): Single header libraries with commonly used data structures.
* [stb](https://github.com/nothings/stb) Single-file public domain libraries for C/C++

Here is my [TODO](https://github.com/acoto87/war1/blob/master/todo.md) list.

## Other Warcraft 1 re-implementations:

* [War1gus](https://wargus.github.io/war1gus.html): War1gus is a re-implementation of "Warcraft: Orcs & Humans" that allows you to play Warcraft with the Stratagus engine.
* [OpenWar](https://phix.itch.io/openwar): OpenWar is an alternative Warcraft: Orcs & Humans game engine.
* [Warcraft Remake](http://www.b3dgs.com/v7/page.php?lang=en&section=warcraft_remake): Warcraft Remake is a remake of the classic Blizzard game.
* [WinWar](https://github.com/CAMongrel/WinWar): WinWar is a multiplatform (Windows, WindowsStore, OSX, Linux, iOS) port of the original DOS WarCraft: Orcs & Humans PC Game.

## Screenshots

Gifs                      |  Images
------------------------- | -------------------------
![](https://github.com/acoto87/war1/blob/master/pics/gif1.gif) | ![](https://github.com/acoto87/war1/blob/master/pics/pic1.png)
![](https://github.com/acoto87/war1/blob/master/pics/gif2.gif) | ![](https://github.com/acoto87/war1/blob/master/pics/pic2.png)
![](https://github.com/acoto87/war1/blob/master/pics/gif3.gif) | ![](https://github.com/acoto87/war1/blob/master/pics/pic3.png)