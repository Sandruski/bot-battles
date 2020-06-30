# Bot Battles
[Bot Battles](https://github.com/Sandruski/bot-battles) is a 2D online multiplayer shooter video game about programming your own bot and battling against others. Two clients compete in a server.

![in_game](https://github.com/Sandruski/bot-battles/blob/master/in_game.png)

# Requirements
- Python 3.8

To use the example bots:
- PyGLM

# Installation
1. Go to [Releases](https://github.com/Sandruski/bot-battles/releases) and download the latest release.
2. Unzip the game and open its folder.
3. Go to bin>bin86 for the 32-bit version or bin>bin64 for the 64-bit version.
4. Run one instance of server.exe and two instances of client.exe.

# How to play
## Create a bot
1. Go to data>scripts>bots.
2. Copy exampleBot.py or create a new .py file in the same folder.
3. Open it in Visual Studio.
4. Go to data>scripts>stubs.
5. Open botbattles.pyi in Visual Studio too.
6. Go to docs.
7. Open botAPI.md.
8. Code your bot.
9. Once you're done, save it.

## Create a map
1. Download [Tiled](https://www.mapeditor.org/) and install it.
2. Go to data>source.
3. Open backyard.tmx or house.tmx or create a new map in Tiled.
4. Edit your map.
5. Once you're done, export it as a .json in data>maps.

## Play a deathmatch
1. On the client, write the IP and the port of the server, your name, and select the bot that you want to play with.
2. On the server, write its port, and select the map that you want to play in and the duration of the game.
3. Wait until the connection between the client and the server is established.

Have fun!

# Controls
- Esc: toggle Settings menu
- F1: toggle Debug Options menu

Only during gameplay:
- O: toggle Log output
- Left Shift + W: end the match

# Known issues

# Changelog
## Vertical Slice
## Alpha
## Beta
## Gold

# About
The application is written in C++ and follows the ISO C++ 17 Standard. The scripts are written in Python.

## Tools
- Microsoft Visual Studio
- Tiled Map Editor
- Aseprite
- Clumsy
- Optick

## Libraries
- SDL
- RapidJSON
- gl3w
- OpenGL Mathematics (GLM)
- STB Image
- Dear ImGui
- Box2D
- Winsock
- pybind11

## Assets
- [Kenney's Topdown Shooter pack](https://www.kenney.nl/assets/topdown-shooter)
- [Kenney's Topdown Tanks Redux pack](https://www.kenney.nl/assets/topdown-tanks-redux)
- [Kenney's Kenney Fonts pack](https://www.kenney.nl/assets/kenney-fonts)

# License
The project is under the MIT License.