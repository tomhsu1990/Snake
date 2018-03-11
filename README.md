# Snake Game
Given a board sized W x H, uniformly generate the snake's start position and a biscuit's position at the beginning of the game. 
Program the Snake game and each time the biscuit should be uniformly randomly placed on the board. 
Besides, each operation should take constant time! There are two modes: player mode and robot mode.

## How to run: (MacOS)
g++ -std=c++11 -pthread Snake.cpp

./a.out map_height map_width

ex: ./a.out 10 10

If you want to run in robot mode,

./a.out 10 10 1

I haven't tested in Linux and Windows, but I guess the code will work fine under Linux.
