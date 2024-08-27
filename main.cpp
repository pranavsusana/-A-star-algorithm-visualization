#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <bits/stdc++.h>

#include <cmath>
#include <functional>
#include <iostream>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>
using namespace std;

const int WIDTH = 800;
const int ROWS = 50;
const int GAP = WIDTH / ROWS;

enum Color {
	RED,
	GREEN,
	BLUE,
	YELLOW,
	WHITE,
	BLACK,
	PURPLE,
	ORANGE,
	GREY,
	TURQUOISE
};

struct Spot {
	int row, col;
	int x, y;
	Color color;
	std::vector<Spot*> neighbors;

	Spot(int r, int c) {
		row = r;
		col = c;
		x = c * GAP;
		y = r * GAP;
		color = WHITE;
	}

	void reset() {
		color = WHITE;
	}

	void make_start() {
		color = ORANGE;
	}

	void make_closed() {
		color = RED;
	}

	void make_open() {
		color = GREEN;
	}

	void make_barrier() {
		color = BLACK;
	}

	void make_end() {
		color = TURQUOISE;
	}

	void make_path() {
		color = PURPLE;
	}

	bool is_closed() const {
		return color == RED;
	}

	bool is_open() const {
		return color == GREEN;
	}

	bool is_barrier() const {
		return color == BLACK;
	}

	bool is_start() const {
		return color == ORANGE;
	}

	bool is_end() const {
		return color == TURQUOISE;
	}

	std::pair<int, int> get_pos() const {
		return {row, col};
	}

	void update_neighbors(std::vector<std::vector<Spot>>& grid) {
		neighbors.clear();
		if(row < ROWS - 1 && !grid[row + 1][col].is_barrier())	// DOWN
			neighbors.push_back(&grid[row + 1][col]);
		if(row > 0 && !grid[row - 1][col].is_barrier())	 // UP
			neighbors.push_back(&grid[row - 1][col]);
		if(col < ROWS - 1 && !grid[row][col + 1].is_barrier())	// RIGHT
			neighbors.push_back(&grid[row][col + 1]);
		if(col > 0 && !grid[row][col - 1].is_barrier())	 // LEFT
			neighbors.push_back(&grid[row][col - 1]);
	}

	void draw() const {
		switch(color) {
			case RED:
				glColor3f(1.0f, 0.0f, 0.0f);
				break;
			case GREEN:
				glColor3f(0.0f, 1.0f, 0.0f);
				break;
			case BLUE:
				glColor3f(0.0f, 1.0f, 0.0f);
				break;
			case YELLOW:
				glColor3f(1.0f, 1.0f, 0.0f);
				break;
			case WHITE:
				glColor3f(1.0f, 1.0f, 1.0f);
				break;
			case BLACK:
				glColor3f(0.0f, 0.0f, 0.0f);
				break;
			case PURPLE:
				glColor3f(0.5f, 0.0f, 0.5f);
				break;
			case ORANGE:
				glColor3f(1.0f, 0.65f, 0.0f);
				break;
			case GREY:
				glColor3f(0.5f, 0.5f, 0.5f);
				break;
			case TURQUOISE:
				glColor3f(0.25f, 0.88f, 0.82f);
				break;
		}
		glBegin(GL_QUADS);
		glVertex2f(x, y);
		glVertex2f(x + GAP, y);
		glVertex2f(x + GAP, y + GAP);
		glVertex2f(x, y + GAP);
		glEnd();
	}
};

int heuristic(const std::pair<int, int>& p1, const std::pair<int, int>& p2) {
	return abs(p1.first - p2.first) + abs(p1.second - p2.second);
}
void clear_previous_path(std::vector<std::vector<Spot>>& grid, std::function<void()> draw) {
	for(auto& row : grid) {
		for(auto& spot : row) {
			if(spot.color != ORANGE && spot.color != TURQUOISE && spot.color != BLACK) {
				spot.reset();
			}
		}
	}
	draw();
}
void reconstruct_path(std::unordered_map<Spot*, Spot*>& came_from, Spot* current, Spot* start, std::function<void()> draw) {
	while(current != start) {
		current->make_path();
		current = came_from[current];
		draw();
	}
}

bool algorithm(std::function<void()> draw, std::vector<std::vector<Spot>>& grid, Spot* start, Spot* end) {
	std::priority_queue<std::tuple<int, int, Spot*>> open_set;
	std::unordered_map<Spot*, Spot*> came_from;
	std::unordered_map<Spot*, int> g_score;
	std::unordered_map<Spot*, int> f_score;
	std::unordered_map<Spot*, int> vis;

	for(auto& row : grid) {
		for(auto& spot : row) {
			g_score[&spot] = INT_MAX;
			f_score[&spot] = INT_MAX;
		}
	}
	g_score[start] = 0;
	f_score[start] = heuristic(start->get_pos(), end->get_pos());
	open_set.emplace(-f_score[start], -g_score[start], start);

	while(!open_set.empty()) {
		Spot* current = std::get<2>(open_set.top());
		int f_s = -std::get<0>(open_set.top());
		open_set.pop();
		if(vis.find(current) != vis.end() || f_s != f_score[current]) {
			continue;
		}
		vis[current] = 1;
		if(current == end) {
			reconstruct_path(came_from, end, start, draw);
			end->make_end();
			return true;
		}

		for(Spot* neighbor : current->neighbors) {
			int temp_g_score = g_score[current] + 1;

			if(temp_g_score < g_score[neighbor]) {
				came_from[neighbor] = current;
				g_score[neighbor] = temp_g_score;
				f_score[neighbor] = temp_g_score + heuristic(neighbor->get_pos(), end->get_pos());
				open_set.emplace(-f_score[neighbor], -g_score[neighbor], neighbor);
				neighbor->make_open();
			}
		}
		draw();
		if(current != start)
			current->make_closed();
	}
	return false;
}

std::vector<std::vector<Spot>> make_grid(int rows) {
	std::vector<std::vector<Spot>> grid(rows, std::vector<Spot>(rows, Spot(0, 0)));
	for(int i = 0; i < rows; ++i) {
		for(int j = 0; j < rows; ++j) {
			grid[i][j] = Spot(i, j);
		}
	}
	return grid;
}

void draw_grid(int rows) {
	glColor3f(0.5f, 0.5f, 0.5f);
	glBegin(GL_LINES);
	for(int i = 0; i < rows; ++i) {
		glVertex2f(0, i * GAP);
		glVertex2f(WIDTH, i * GAP);
		glVertex2f(i * GAP, 0);
		glVertex2f(i * GAP, WIDTH);
	}
	glEnd();
}

void draw(const std::vector<std::vector<Spot>>& grid) {
	glClear(GL_COLOR_BUFFER_BIT);
	for(const auto& row : grid) {
		for(const auto& spot : row) {
			spot.draw();
		}
	}
	draw_grid(ROWS);
	SDL_GL_SwapWindow(SDL_GL_GetCurrentWindow());
}

std::pair<int, int> get_clicked_pos(int x, int y) {
	return {y / GAP, x / GAP};
}

int main(int argc, char* argv[]) {
	if(SDL_Init(SDL_INIT_VIDEO) != 0) {
		std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return 1;
	}

	SDL_Window* win = SDL_CreateWindow("A* Path Finding Algorithm", 100, 100, WIDTH, WIDTH, SDL_WINDOW_OPENGL);
	if(win == nullptr) {
		std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return 1;
	}

	SDL_GLContext glContext = SDL_GL_CreateContext(win);
	if(glContext == nullptr) {
		std::cerr << "SDL_GL_CreateContext Error: " << SDL_GetError() << std::endl;
		SDL_DestroyWindow(win);
		SDL_Quit();
		return 1;
	}

	glewExperimental = GL_TRUE;
	if(glewInit() != GLEW_OK) {
		std::cerr << "GLEW Initialization Error!" << std::endl;
		SDL_GL_DeleteContext(glContext);
		SDL_DestroyWindow(win);
		SDL_Quit();
		return 1;
	}

	// glMatrixMode(GL_PROJECTION);
	// glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	gluOrtho2D(0.0, WIDTH, WIDTH, 0.0);

	auto grid = make_grid(ROWS);
	Spot* start = nullptr;
	Spot* end = nullptr;
	bool run = true;

	while(run) {
		draw(grid);
		SDL_Event e;
		while(SDL_PollEvent(&e)) {
			if(e.type == SDL_QUIT) {
				run = false;
			}

			if(e.type == SDL_MOUSEBUTTONDOWN) {
				int x, y;
				SDL_GetMouseState(&x, &y);
				auto [row, col] = get_clicked_pos(x, y);
				Spot& spot = grid[row][col];

				if(e.button.button == SDL_BUTTON_LEFT) {
					if(!start && spot.color != TURQUOISE) {
						start = &spot;
						start->make_start();
					} else if(!end && spot.color != ORANGE) {
						end = &spot;
						end->make_end();
					} else if(spot.color != ORANGE && spot.color != TURQUOISE) {
						spot.make_barrier();
					}
				} else if(e.button.button == SDL_BUTTON_RIGHT) {
					spot.reset();
					if(&spot == start) {
						start = nullptr;
					} else if(&spot == end) {
						end = nullptr;
					}
				}
			}

			if(e.type == SDL_KEYDOWN) {
				if(e.key.keysym.sym == SDLK_SPACE && start && end) {
					for(auto& row : grid) {
						for(auto& spot : row) {
							spot.update_neighbors(grid);
						}
					}
					clear_previous_path(grid, [&]() { draw(grid); });
					algorithm([&]() { draw(grid); }, grid, start, end);
				}

				if(e.key.keysym.sym == SDLK_c) {
					start = nullptr;
					end = nullptr;
					grid = make_grid(ROWS);
				}
			}
		}
	}

	SDL_GL_DeleteContext(glContext);
	SDL_DestroyWindow(win);
	SDL_Quit();

	return 0;
}

