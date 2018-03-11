#include <stdio.h>
#include <termios.h>
#include <math.h>
#include <unistd.h>

#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <deque>
#include <unordered_set>
#include <unordered_map>
#include <random>
#include <thread>
#include <cfloat>

bool terminate(false), robot(false);
char cmd;

/////////////////////////////////////////////////////////////
// non-enter getchar call
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
/////////////////////////////////////////////////////////////

class Snake {
public:
	using Location = std::string;
	using LocationXY = std::pair<int, int>;
	Snake () {W=H=0;}
	Snake (int w, int h):W(w), H(h) {}

	std::vector<int> parseLocation (std::string s) {
		std::vector<int> loc;
		std::string tmp("");
		for (unsigned i=0;i<s.size();++i) {
			if (s[i] == ',') {
				loc.push_back(atoi(tmp.c_str()));
				tmp = "";
			}
			else tmp.append(1, s[i]);
		}
		return loc;
	}

	void assignNewBiscuit () {
		if (map_index.size() > 0)
			biscuit = map_location[get_random(map_index.size())];
	}

	void initial () {
		for (int h=0;h<H;++h)
			for (int w=0;w<W;++w) {
				Location loc(std::to_string(h)+","+std::to_string(w)+",");
				map_index[loc] = h*H+w;
				map_location[h*H+w] = loc;
			}

		// starting location
		int idx = get_random(map_index.size());
		Location start = map_location[idx];
		auto idx_it = map_index.find(start);
		auto last_it = map_location.find(map_index.size()-1);
		map_index[last_it->second] = idx;
		map_location[idx] = last_it->second;
		map_index.erase(idx_it);
		map_location.erase(last_it);

		body.insert(start);
		std::vector<int> loc = parseLocation(start);
		head.push_back({loc[0], loc[1]});

		// biscuit location
		assignNewBiscuit();
	}

	void next_location () {
		char c[5] = "wads";
		int dx[4] = {-1,0,0,1};
		int dy[4] = {0,-1,1,0};
		int f(0), dist(INT_MAX);
		std::vector<int> loc = parseLocation(biscuit);
		for (int i=0;i<4;++i) {
			int tx(head.front().first+dx[i]);
			int ty(head.front().second+dy[i]);
			if (tx < 0 || tx >= H || ty < 0 || ty >= W) continue;
			if (body.find(std::to_string(tx)+","+std::to_string(ty)+",") != body.end()) continue;

			int dist_(abs(tx-loc[0])+abs(ty-loc[1]));
			if (dist_ < dist) {
				dist = dist_;
				f = i;
			}
		}
		cmd = c[f];
	}

	void show () {
		system("clear");

		std::vector<int> loc;
		std::vector<std::string> mp(H);
		for (unsigned i=0;i<H;++i) mp[i].resize(W);
		
		for (auto it=map_location.begin();it!=map_location.end();++it) {
			loc = parseLocation(it->second);
			mp[loc[0]][loc[1]] = ' ';
		}
		for (auto it=body.begin();it!=body.end();++it) {
			loc = parseLocation(*it);
			mp[loc[0]][loc[1]] = 'x';
		}
		loc = parseLocation(biscuit);
		mp[loc[0]][loc[1]] = 'o';


		for (unsigned i=0;i<W+2;++i) std::cout << "-";
		std::cout << std::endl;
		for (unsigned i=0;i<H;++i)
			std::cout << "|" << mp[i] << "|" << std::endl;
		for (unsigned i=0;i<W+2;++i) std::cout << "-";
		std::cout << std::endl;

		usleep(200000);
	}

	int report_length () {
		return body.size();
	}

	// randomly pick an index from 0~size-1
	int get_random (int size) {
        std::uniform_int_distribution<int> distribution(0, size-1);
        return distribution(gen);
    }

	int W, H;
	Location biscuit;
	std::deque<LocationXY> head;
	std::default_random_engine gen;
	std::unordered_set<Location> body;
	std::unordered_map<Location, int> map_index;
	std::unordered_map<int, Location> map_location;
};

Snake *snake;

void keyboard_listener () {
	while (!terminate) {
		cmd = getch();
		if (cmd == 'q') terminate = true;
	}
}

void run_game () {
	std::unordered_map<char, std::pair<int, int>> dir;
	dir['w'] = {-1, 0}; dir['s'] = {1, 0};
	dir['a'] = {0, -1}; dir['d'] = {0, 1};
	snake->show();
	if (!robot) cmd = getchar();
	while (snake->map_index.size() > 0) {
		snake->show();

		if (robot) {
			snake->next_location();
		}

		int nx(snake->head.front().first+dir[cmd].first);
		int ny(snake->head.front().second+dir[cmd].second);
		Snake::Location nxt(std::to_string(nx)+","+std::to_string(ny)+",");

		// check boundary
		if (nx < 0 || nx >= snake->H ||
			ny < 0 || ny >= snake->W) {
			std::cout << "out of boundary" << std::endl;
			break;
		}

		// check snake's body (need to make sure whether there is a biscuit or not)
		Snake::Location tail_loc(std::to_string(snake->head.back().first)+","+std::to_string(snake->head.back().second)+",");
		int tail_idx(-1);
		if (nxt.compare(snake->biscuit) != 0) {
			auto tail_it = snake->body.find(tail_loc);
			snake->body.erase(tail_it);
			snake->head.pop_back();
		}
		if (snake->body.find(nxt) != snake->body.end()) {
			std::cout << "snake overlapped" << std::endl;
			break;
		}

		// put the move
		snake->body.insert(nxt);
		snake->head.push_front({nx, ny});

		// update available position
		if (nxt.compare(snake->biscuit) != 0) {
			// tail location becomes available
			snake->map_index[tail_loc] = snake->map_index[nxt];
			snake->map_location[snake->map_index[tail_loc]] = tail_loc;
			// nxt location becomes NOT available
			auto tail_idx_it = snake->map_index.find(nxt);
			snake->map_index.erase(tail_idx_it);
		}
		else {
			auto last_it = snake->map_location.find(snake->map_index.size()-1);
			// nxt location becomes NOT available
			auto tail_idx_it = snake->map_index.find(nxt);
			snake->map_location[tail_idx_it->second] = last_it->second;
			snake->map_index[last_it->second] = tail_idx_it->second;

			snake->map_index.erase(tail_idx_it);
			snake->map_location.erase(last_it);

			snake->assignNewBiscuit();
		}

		if (cmd == 'q') break;
	}
	terminate = true;
}

int main (int argc, char *argv[]) {

	int w, h;
	if (argc >= 1) w = h = 10;
	if (argc >= 2) w = h = atoi(argv[1]);
	if (argc >= 3) {
		w = atoi(argv[1]);
		h = atoi(argv[2]);
	}
	if (argc >= 4 && atoi(argv[3]) > 0) {
		terminate = robot = true;
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
