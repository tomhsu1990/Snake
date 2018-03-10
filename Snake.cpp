#include <stdio.h>
#include <math.h>

#include <iostream>
#include <fstream>
#include <string>
#include <deque>
#include <unordered_map>
#include <random>

class Snake {
public:
	using Location = std::string;
	Snake () {W=H=0;}
	Snake (int w, int h):W(w), H(h) {}

	void initial () {

	}

	void run () {

	}

	int report_length () {
		return 0;
	}

	// randomly pick an index from 0~size-1
	int get_random (int size) {
        std::uniform_int_distribution<int> distribution(0, size-1);
        return distribution(gen);
    }

	int W, H;
	std::default_random_engine gen;
	std::unordered_map<Location, int> map_index;
	std::unordered_map<int, Location> map_location;
};

int main (int argc, char *argv[]) {

	int w, h;
	if (argc == 1) w = h = 10;
	else if (argc == 2) w = h = atoi(argv[1]);
	else {
		w = atoi(argv[1]);
		h = atoi(argv[2]);
	}

	Snake *snake = new Snake(w, h);
	snake->initial ();
	snake->run ();
	std::cout << "Final Snake length: " << snake->report_length () << std::endl;

	return 0;
}
