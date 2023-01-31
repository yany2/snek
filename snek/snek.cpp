﻿#include <stdio.h>
#include <stdlib.h>

#include <chrono>
#include <queue>

#include <sdl.h>

enum Texture{
	HEAD, BODY, TAIL, TURN, BACK, FOOD_TEXTURE
};

enum TileType{
	SNEK, EMPTY, FOOD
};

enum Direction{
	UP, RIGHT, DOWN, LEFT, NONE
};

struct Snek{
	int x;
	int y;
	Snek* next;
};

struct Tile{
	TileType type;
	Direction in;
	Direction out;
};

SDL_Texture** textures;

SDL_Window* window;
SDL_Renderer* renderer;

Tile** world;
Snek* snekFront;
Direction snekDirection = UP;

std::queue<Direction> inputQueue;

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

	world = new Tile* [WIDTH];
	for (int i = 0; i < WIDTH; i++) {
		world[i] = new Tile[HEIGHT];
		for (int j = 0; j < HEIGHT; j++) {
			world[i][j].type = EMPTY;
			world[i][j].in = NONE;
			world[i][j].out = NONE;
		}
	}
	snekFront = new Snek;
	snekFront->x = WIDTH / 2;
	snekFront->y = HEIGHT / 2;
	snekFront->next = new Snek;
	snekFront->next->x = snekFront->x - 1;
	snekFront->next->y = snekFront->y;
	snekFront->next->next = new Snek;
	snekFront->next->next->x = snekFront->x - 2;
	snekFront->next->next->y = snekFront->y;
	snekFront->next->next->next = nullptr;

	world[snekFront->x][snekFront->y].type = SNEK;
	world[snekFront->next->x][snekFront->next->y].type = SNEK;
	world[snekFront->next->next->x][snekFront->next->next->y].type = SNEK;

	world[snekFront->x][snekFront->y].in = RIGHT;
	world[snekFront->next->x][snekFront->next->y].in = RIGHT;
	world[snekFront->next->next->x][snekFront->next->next->y].out = RIGHT;
	world[snekFront->next->x][snekFront->next->y].out = RIGHT;

	int newFoodX, newFoodY;
	for (int i = 0; i < FOOD_COUNT; i++) {
		do {
			newFoodX = rand() % WIDTH;
			newFoodY = rand() % HEIGHT;
		} while (world[newFoodX][newFoodY].type != EMPTY);
		world[newFoodX][newFoodY].type = FOOD;
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
		SDL_Event event;
		SDL_PollEvent(&event);
		switch (event.type) {
		case SDL_QUIT:
			running = false;
			break;
		case SDL_KEYDOWN:
			switch (event.key.keysym.scancode) {
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
		SDL_Rect renderTarget = { 0, 0, FIELD, FIELD };


		count--;
		if (!count) {
			Snek* nextTail = snekFront;
			do {
				nextTail = nextTail->next;
			} while (nextTail->next->next);
			count = 15;
			int previousTailX = nextTail->next->x;
			int previousTailY = nextTail->next->y;
			nextTail->next->next = snekFront;
			world[nextTail->next->x][nextTail->next->y].type = EMPTY;
			world[nextTail->next->x][nextTail->next->y].out = NONE;
			Direction previousDirection = world[nextTail->x][nextTail->y].in;
			world[nextTail->x][nextTail->y].in = NONE;
			nextTail->next->x = snekFront->x;
			nextTail->next->y = snekFront->y;
			snekFront = nextTail->next;
			nextTail->next = nullptr;

			while (!inputQueue.empty() && (inputQueue.front() == snekDirection || (inputQueue.front() + 2) % 4 == snekDirection))
				inputQueue.pop();
			if (!inputQueue.empty()) {
				snekDirection = inputQueue.front();
				inputQueue.pop();
			}

			switch (snekDirection) {
			case UP:
				snekFront->y--;
				if (snekFront->y < 0)
					snekFront->y = HEIGHT - 1;
				break;
			case DOWN:
				snekFront->y++;
				if (snekFront->y >= HEIGHT)
					snekFront->y = 0;
				break;
			case LEFT:
				snekFront->x--;
				if (snekFront->x < 0)
					snekFront->x = WIDTH - 1;
				break;
			case RIGHT:
				snekFront->x++;
				if (snekFront->x >= WIDTH)
					snekFront->x = 0;
				break;
			}
			world[snekFront->x][snekFront->y].in = snekDirection;
			world[snekFront->next->x][snekFront->next->y].out = snekDirection;

			if (world[snekFront->x][snekFront->y].type == FOOD) {
				nextTail->next = new Snek;
				nextTail->next->x = previousTailX;
				nextTail->next->y = previousTailY;
				nextTail->next->next = nullptr;
				world[nextTail->next->x][nextTail->next->y].type = SNEK;
				world[nextTail->x][nextTail->y].in = previousDirection;
				world[nextTail->next->x][nextTail->next->y].out = previousDirection;


				int newFoodX, newFoodY;
				do {
					newFoodX = rand() % WIDTH;
					newFoodY = rand() % HEIGHT;
				} while (world[newFoodX][newFoodY].type != EMPTY);
				world[newFoodX][newFoodY].type = FOOD;
			}

			running = world[snekFront->x][snekFront->y].type != SNEK;
			world[snekFront->x][snekFront->y].type = SNEK;
		}


		for (int i = 0; i < WIDTH; i++) {
			for (int j = 0; j < HEIGHT; j++) {
				renderTarget.x = i * FIELD;
				renderTarget.y = j * FIELD;
				switch (world[i][j].type) {
				case EMPTY:
					SDL_RenderCopy(renderer, textures[BACK], nullptr, &renderTarget);
					break;
				case SNEK:
					if (world[i][j].out == NONE)
						switch (world[i][j].in) {
						case RIGHT:
							SDL_RenderCopy(renderer, textures[HEAD], nullptr, &renderTarget);
							break;
						case LEFT:
							SDL_RenderCopyEx(renderer, textures[HEAD], nullptr, &renderTarget, 0, nullptr, SDL_FLIP_HORIZONTAL);
							break;
						case UP:
							SDL_RenderCopyEx(renderer, textures[HEAD], nullptr, &renderTarget, 270, nullptr, SDL_FLIP_NONE);
							break;
						case DOWN:
							SDL_RenderCopyEx(renderer, textures[HEAD], nullptr, &renderTarget, 90, nullptr, SDL_FLIP_NONE);
							break;
						}

					if ((world[i][j].out == UP && world[i][j].in == UP) || (world[i][j].out == DOWN && world[i][j].in == DOWN)) {
						SDL_RenderCopyEx(renderer, textures[BODY], nullptr, &renderTarget, 90, nullptr, SDL_FLIP_NONE);
					}
					if ((world[i][j].out == LEFT && world[i][j].in == LEFT) || (world[i][j].out == RIGHT && world[i][j].out == RIGHT))
						SDL_RenderCopy(renderer, textures[BODY], nullptr, &renderTarget);



					if ((world[i][j].in == UP && world[i][j].out == RIGHT) || (world[i][j].in == LEFT && world[i][j].out == DOWN))
						SDL_RenderCopy(renderer, textures[TURN], nullptr, &renderTarget);
					if ((world[i][j].in == DOWN && world[i][j].out == RIGHT) || (world[i][j].in == LEFT && world[i][j].out == UP))
						SDL_RenderCopyEx(renderer, textures[TURN], nullptr, &renderTarget, 0, nullptr, SDL_FLIP_VERTICAL);
					if ((world[i][j].in == UP && world[i][j].out == LEFT) || (world[i][j].in == RIGHT && world[i][j].out == DOWN))
						SDL_RenderCopyEx(renderer, textures[TURN], nullptr, &renderTarget, 0, nullptr, SDL_FLIP_HORIZONTAL);
					if ((world[i][j].in == RIGHT && world[i][j].out == UP) || (world[i][j].in == DOWN && world[i][j].out == LEFT)) {
						SDL_RenderCopyEx(renderer, textures[TURN], nullptr, &renderTarget, 180, nullptr, SDL_FLIP_NONE);
					}


					if (world[i][j].in == NONE)
						switch (world[i][j].out) {
						case RIGHT:
							SDL_RenderCopy(renderer, textures[TAIL], nullptr, &renderTarget);
							break;
						case LEFT:
							SDL_RenderCopyEx(renderer, textures[TAIL], nullptr, &renderTarget, 0, nullptr, SDL_FLIP_HORIZONTAL);
							break;
						case UP:
							SDL_RenderCopyEx(renderer, textures[TAIL], nullptr, &renderTarget, 270, nullptr, SDL_FLIP_NONE);
							break;
						case DOWN:
							SDL_RenderCopyEx(renderer, textures[TAIL], nullptr, &renderTarget, 90, nullptr, SDL_FLIP_NONE);
							break;
						}

					break;
				case FOOD:
					SDL_RenderCopy(renderer, textures[FOOD_TEXTURE], nullptr, &renderTarget);
					break;
				}
			}
		}
		SDL_RenderPresent(renderer);


	}

	close();
	return 0;
}