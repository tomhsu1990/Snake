# Snake Game
Given a board sized W x H, uniformly generate the snake's start position and a biscuit's position at the beginning of the game. 
Program the Snake game and each time the biscuit should be uniformly randomly placed on the board. 
Besides, each operation should take constant time! There are two modes: player mode and robot mode.

Here is the demonstration video: [Snake Game Demo](https://youtu.be/qkfW57sHXrc)

## Idea
How to achieve constant biscuit location update is based on two unordered_map: map_index<Location, int> and map_location<int, location>. Basically they maintain the same thing, location and the index for uniform sampling. Suppose the range of the sampling space is from "0 - N-1". If location "loc" is removed, we use "map_index[loc]" to get its index "d". In order to shrink the sampling space, we use "map_location[N-1]" to find the last location "last_loc" and update this location's index via "map_index[last_loc] = d". Then, we perform the deletion of "map_location[N-1]" and "map_index[loc]". This way, the sampling space becomes "0 - N-2". In addition, we can apply "uniform_int_distribution" to randomly compute an index in this range.

## Discussion
When implementing this small command line game, the most tricky part is to program a keyboard handler. The way I solve it is via multi-thread. One thread is for running the game. The other thread is for listening the keyboard command. One more step is to have the getchar function working without hitting the enter. This problem is solved via termios.

## How to run: (MacOS)
g++ -std=c++11 -pthread Snake.cpp

./a.out map_height map_width

ex: ./a.out 10 10

If you want to run in robot mode,

./a.out 10 10 1

I haven't tested in Linux and Windows, but I guess the code will work fine under Linux.
