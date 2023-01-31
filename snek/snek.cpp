#include <stdio.h>
#include <stdlib.h>

#include <chrono>
#include <queue>

#include <sdl.h>

enum {
	HEAD, BODY, TAIL, TURN, BACK, FOOD_TEXTURE
};

enum {
	SNEK, EMPTY, FOOD
};

enum {
	UP, RIGHT, DOWN, LEFT, NONE
};

struct Snek {
	int x;
	int y;
	Snek* next;
};

SDL_Texture** textures;

Snek* front;
SDL_Window* window;
SDL_Renderer* renderer;
char** world;
char** worlddirin;
char** worlddirout;
char dir = UP;
std::queue<char> inputQueue;

const int FIELD = 32;
const int WIDTH = 20;
const int HEIGHT = 20;
const int FOOD_COUNT = 3;



void init() {
	SDL_Init(SDL_INIT_VIDEO);
	window = SDL_CreateWindow("snek", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH * FIELD, HEIGHT * FIELD, SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
	textures = new SDL_Texture * [6];
	textures[HEAD] = SDL_CreateTextureFromSurface(renderer, SDL_LoadBMP("textures/head.bmp"));
	textures[BODY] = SDL_CreateTextureFromSurface(renderer, SDL_LoadBMP("textures/body.bmp"));
	textures[BACK] = SDL_CreateTextureFromSurface(renderer, SDL_LoadBMP("textures/back.bmp"));
	textures[FOOD_TEXTURE] = SDL_CreateTextureFromSurface(renderer, SDL_LoadBMP("textures/food.bmp"));
	textures[TURN] = SDL_CreateTextureFromSurface(renderer, SDL_LoadBMP("textures/turn.bmp"));
	textures[TAIL] = SDL_CreateTextureFromSurface(renderer, SDL_LoadBMP("textures/tail.bmp"));

	world = new char* [WIDTH];
	for (int i = 0; i < WIDTH; i++) {
		world[i] = new char[HEIGHT];
		for (int j = 0; j < HEIGHT; j++) {
			world[i][j] = EMPTY;
		}
	}
	front = new Snek;
	front->x = WIDTH / 2;
	front->y = HEIGHT / 2;
	front->next = new Snek;
	front->next->x = front->x - 1;
	front->next->y = front->y;
	front->next->next = new Snek;
	front->next->next->x = front->x - 2;
	front->next->next->y = front->y;
	front->next->next->next = nullptr;

	world[front->x][front->y] =
		world[front->next->x][front->next->y] =
		world[front->next->next->x][front->next->next->y] = SNEK;

	worlddirin = new char* [WIDTH];
	worlddirout = new char* [WIDTH];
	for (int i = 0; i < WIDTH; i++) {
		worlddirin[i] = new char[HEIGHT];
		worlddirout[i] = new char[HEIGHT];
		for (int j = 0; j < HEIGHT; j++) {
			worlddirin[i][j] = worlddirout[i][j] = NONE;
		}
	}


	worlddirin[front->x][front->y] =
		worlddirin[front->next->x][front->next->y] = RIGHT;
	worlddirout[front->next->next->x][front->next->next->y] =
		worlddirout[front->next->x][front->next->y] = RIGHT;

	int nx, ny;
	for (int i = 0; i < FOOD_COUNT; i++) {
		do {
			nx = rand() % WIDTH;
			ny = rand() % HEIGHT;
		} while (world[nx][ny] != EMPTY);
		world[nx][ny] = FOOD;
	}

	srand(time(0));
}

void close() {
	for (int i = 0; i < 6; i++) {
		SDL_DestroyTexture(textures[i]);
	}
	delete[] textures;
	for (int i = 0; i < WIDTH; i++) {
		delete[] world[i];
	}
	delete[] world;
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

int main(int argc, char** argv) {
	init();
	bool running = true;
	int count = 60;
	while (running) {
		SDL_Event e;
		SDL_PollEvent(&e);
		switch (e.type) {
		case SDL_QUIT:
			running = false;
			break;
		case SDL_KEYDOWN:
			switch (e.key.keysym.scancode) {
			case SDL_SCANCODE_UP:
				inputQueue.push(UP);
				break;
			case SDL_SCANCODE_DOWN:
				inputQueue.push(DOWN);
				break;
			case SDL_SCANCODE_LEFT:
				inputQueue.push(LEFT);
				break;
			case SDL_SCANCODE_RIGHT:
				inputQueue.push(RIGHT);
				break;
			}
			break;
		}


		SDL_RenderClear(renderer);
		SDL_Rect a = { 0, 0, FIELD, FIELD };


		count--;
		if (!count) {
			Snek* nexttail = front;
			do {
				nexttail = nexttail->next;
			} while (nexttail->next->next);
			count = 15;
			int prevtailx = nexttail->next->x;
			int prevtaily = nexttail->next->y;
			nexttail->next->next = front;
			world[nexttail->next->x][nexttail->next->y] = EMPTY;
			worlddirout[nexttail->next->x][nexttail->next->y] = NONE;
			char prevdir = worlddirin[nexttail->x][nexttail->y];
			worlddirin[nexttail->x][nexttail->y] = NONE;
			nexttail->next->x = front->x;
			nexttail->next->y = front->y;
			front = nexttail->next;
			nexttail->next = nullptr;

			while (!inputQueue.empty() && (inputQueue.front() == dir || (inputQueue.front() + 2) % 4 == dir))
				inputQueue.pop();
			if (!inputQueue.empty()) {
				dir = inputQueue.front();
				inputQueue.pop();
			}

			switch (dir) {
			case UP:
				front->y--;
				if (front->y < 0)
					front->y = HEIGHT - 1;
				break;
			case DOWN:
				front->y++;
				if (front->y >= HEIGHT)
					front->y = 0;
				break;
			case LEFT:
				front->x--;
				if (front->x < 0)
					front->x = WIDTH - 1;
				break;
			case RIGHT:
				front->x++;
				if (front->x >= WIDTH)
					front->x = 0;
				break;
			}
			worlddirin[front->x][front->y] = dir;
			worlddirout[front->next->x][front->next->y] = dir;

			if (world[front->x][front->y] == FOOD) {
				nexttail->next = new Snek;
				nexttail->next->x = prevtailx;
				nexttail->next->y = prevtaily;
				nexttail->next->next = nullptr;
				world[nexttail->next->x][nexttail->next->y] = SNEK;
				worlddirin[nexttail->x][nexttail->y] = worlddirout[nexttail->next->x][nexttail->next->y] = prevdir;


				int nx, ny;
				do {
					nx = rand() % WIDTH;
					ny = rand() % HEIGHT;
				} while (world[nx][ny] != EMPTY);
				world[nx][ny] = FOOD;
			}

			running = world[front->x][front->y] != SNEK;
			world[front->x][front->y] = SNEK;
		}


		for (int i = 0; i < WIDTH; i++) {
			for (int j = 0; j < HEIGHT; j++) {
				a.x = i * FIELD;
				a.y = j * FIELD;
				switch (world[i][j]) {
				case EMPTY:
					SDL_RenderCopy(renderer, textures[BACK], nullptr, &a);
					break;
				case SNEK:
					if (worlddirout[i][j] == NONE)
						switch (worlddirin[i][j]) {
						case RIGHT:
							SDL_RenderCopy(renderer, textures[HEAD], nullptr, &a);
							break;
						case LEFT:
							SDL_RenderCopyEx(renderer, textures[HEAD], nullptr, &a, 0, nullptr, SDL_FLIP_HORIZONTAL);
							break;
						case UP:
							SDL_RenderCopyEx(renderer, textures[HEAD], nullptr, &a, 270, nullptr, SDL_FLIP_NONE);
							break;
						case DOWN:
							SDL_RenderCopyEx(renderer, textures[HEAD], nullptr, &a, 90, nullptr, SDL_FLIP_NONE);
							break;
						}

					if ((worlddirout[i][j] == UP && worlddirin[i][j] == UP) || (worlddirout[i][j] == DOWN && worlddirin[i][j] == DOWN)) {
						SDL_RenderCopyEx(renderer, textures[BODY], nullptr, &a, 90, nullptr, SDL_FLIP_NONE);
					}
					if ((worlddirout[i][j] == LEFT && worlddirin[i][j] == LEFT) || (worlddirout[i][j] == RIGHT && worlddirin[i][j] == RIGHT))
						SDL_RenderCopy(renderer, textures[BODY], nullptr, &a);



					if ((worlddirin[i][j] == UP && worlddirout[i][j] == RIGHT) || (worlddirin[i][j] == LEFT && worlddirout[i][j] == DOWN))
						SDL_RenderCopy(renderer, textures[TURN], nullptr, &a);
					if ((worlddirin[i][j] == DOWN && worlddirout[i][j] == RIGHT) || (worlddirin[i][j] == LEFT && worlddirout[i][j] == UP))
						SDL_RenderCopyEx(renderer, textures[TURN], nullptr, &a, 0, nullptr, SDL_FLIP_VERTICAL);
					if ((worlddirin[i][j] == UP && worlddirout[i][j] == LEFT) || (worlddirin[i][j] == RIGHT && worlddirout[i][j] == DOWN))
						SDL_RenderCopyEx(renderer, textures[TURN], nullptr, &a, 0, nullptr, SDL_FLIP_HORIZONTAL);
					if ((worlddirin[i][j] == RIGHT && worlddirout[i][j] == UP) || (worlddirin[i][j] == DOWN && worlddirout[i][j] == LEFT)) {
						SDL_RenderCopyEx(renderer, textures[TURN], nullptr, &a, 180, nullptr, SDL_FLIP_NONE);
					}


					if (worlddirin[i][j] == NONE)
						switch (worlddirout[i][j]) {
						case RIGHT:
							SDL_RenderCopy(renderer, textures[TAIL], nullptr, &a);
							break;
						case LEFT:
							SDL_RenderCopyEx(renderer, textures[TAIL], nullptr, &a, 0, nullptr, SDL_FLIP_HORIZONTAL);
							break;
						case UP:
							SDL_RenderCopyEx(renderer, textures[TAIL], nullptr, &a, 270, nullptr, SDL_FLIP_NONE);
							break;
						case DOWN:
							SDL_RenderCopyEx(renderer, textures[TAIL], nullptr, &a, 90, nullptr, SDL_FLIP_NONE);
							break;
						}

					break;
				case FOOD:
					SDL_RenderCopy(renderer, textures[FOOD_TEXTURE], nullptr, &a);
					break;
				}
			}
		}
		SDL_RenderPresent(renderer);


	}

	close();
	return 0;
}