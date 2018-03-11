#include <stdio.h>
#include <termios.h>
#include <math.h>

#include <iostream>
#include <fstream>
#include <string>
#include <deque>
#include <unordered_set>
#include <unordered_map>
#include <random>
#include <thread>

bool terminate(false);
char cmd;

static struct termios old_t, new_t;

/* Initialize new terminal i/o settings */
void initTermios (int echo) {
	tcgetattr(0, &old_t); /* grab old terminal i/o settings */
	new_t = old_t; /* make new settings same as old settings */
	new_t.c_lflag &= ~ICANON; /* disable buffered i/o */
	new_t.c_lflag &= echo ? ECHO : ~ECHO; /* set echo mode */
	tcsetattr(0, TCSANOW, &new_t); /* use these new terminal i/o settings now */
}
/* Restore old terminal i/o settings */
void resetTermios (void) {
	tcsetattr(0, TCSANOW, &old_t);
}
/* Read 1 character - echo defines echo mode */
char getch_ (int echo) {
	char ch;
	initTermios(echo);
	ch = getchar();
	resetTermios();
	return ch;
}
/* Read 1 character without echo */
char getch (void) {
  return getch_(0);
}
/* Read 1 character with echo */
char getche(void) {
	return getch_(1);
}

class Snake {
public:
	using Location = std::string;
	Snake () {W=H=0;}
	Snake (int w, int h):W(w), H(h) {}

	void initial () {
		for (int w=0;w<W;++w)
			for (int h=0;h<H;++h) {
				Location loc(std::to_string(w)+","+std::to_string(h)+",");
				map_index[loc] = w*W+h;
				map_location[w*W+h] = loc;
			}

		// starting location
		int idx = get_random(map_index.size());
		Location start = map_location[idx];
		auto idx_it = map_index.find(start);
		auto loc_it = map_location.find(idx_it->second);
		map_index.erase(idx_it);
		map_location.erase(loc_it);

		body.insert(start);
		head = tail = start;

		// biscuit location
		biscuit = map_location[get_random(map_index.size())];
	}

	void show () {
		std::vector<std::string> mp(W);
		for (unsigned i=0;i<W;++i) mp[i].resize(H);
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
	Location biscuit, head, tail;
	std::default_random_engine gen;
	std::unordered_set<Location> body;
	std::unordered_map<Location, int> map_index;
	std::unordered_map<int, Location> map_location;
};

Snake *snake;

void keyboard_listener () {
	while (!terminate) {
		cmd = getch();
		if (cmd == 'q') {
			terminate = true;
			break;
		}
	}
}

void run_game () {
	snake->show();
	cmd = getchar();
	while (snake->map_index.size() > 0) {
		if (cmd == 'q') {
			terminate = true;
			break;
		}
	}
}

int main (int argc, char *argv[]) {

	int w, h;
	if (argc == 1) w = h = 10;
	else if (argc == 2) w = h = atoi(argv[1]);
	else {
		w = atoi(argv[1]);
		h = atoi(argv[2]);
	}

	snake = new Snake(w, h);
	snake->initial();
	//Launch a keybaord thread
	std::thread keyboard_thread(keyboard_listener);
	//Launch a snake thread
	std::thread game_thread(run_game);
	//Join the thread with the main thread
	keyboard_thread.join();
	game_thread.join();
	std::cout << "Final Snake length: " << snake->report_length () << std::endl;
	delete snake;

	return 0;
}
