# 3D-Tetris
Created as a project for UNSW COMP1511 course.<br>
Fully functional traditional tetris with 3D visuals using OpenGL.

## Dependencies
* OpenGL
* Make
* Clang or GCC
* GLFW3 : [Click here to download](http://www.glfw.org/)
    * On Mac can be downloaded via Hombrew with `brew install glfw3`
    * On Ubuntu can be downloaded via `sudo apt-get install glfw3`
* Freetype2 : [Click here to download](https://www.freetype.org/)
    * On Mac can be downloaded via Homebrew with `brew install freetype`
    * [Tutorial for installing on Ubuntu](http://ubuntuhandbook.org/index.php/2017/06/install-freetype-2-8-in-ubuntu-16-04-17-04/)
* CMake : [Click here to download](https://cmake.org/)
    * On Mac can be downloaded via Homebrew with `brew install cmake`
    * [Tutorial for installing on Ubuntu](https://askubuntu.com/questions/610291/how-to-install-cmake-3-2-on-ubuntu)
* OpenAL Soft : [Click here to download](https://github.com/kcat/openal-soft)
    * On Mac can be downloaded via Hombrew with `brew install openal-soft`
    * On Ubuntu can be downloaded via `sudo apt-get install openal-soft`

## Installation instructions
1. Install all of the dependencies above if you have not already.
2. Clone the repository via the command line with `git clone https://github.com/Balajanovski/3D-Tetris.git`
3. Navigate to the repository root directory
4. Type `cmake .` to generate a makefile
5. Type `make` to build the project
6. Run the game by typing `./3d-tetris`

## Controls
**Left Arrow** : Move tetromino left<br>
**Right Arrow** : Move tetromino right<br>
**Up Arrow** : Rotate tetromino<br>
**Down Arrow** : Move tetromino down quicker<br>
**Space Bar** : Drop tetromino<br>
**Esc Key** : Escape game<br>
**P key** : Pause game<br>
**R key** : Reset game<br>

## Screenshot of Gameplay

![Screenshot of Gameplay](https://raw.githubusercontent.com/Balajanovski/3D-Tetris/master/screenshot.png)
