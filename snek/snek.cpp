#include <stdio.h>
#include <stdlib.h>

#include <chrono>
#include <queue>

#include <SDL.h>

enum Texture {
	HEAD, BODY, TAIL, TURN, BACK, FOOD_TEXTURE
};

enum TileType {
	SNEK, EMPTY, FOOD
};

enum Direction {
	UP, RIGHT, DOWN, LEFT, NONE
};

struct Snek {
	int x;
	int y;
	Snek* next;
};

struct Tile {
	TileType type;
	Direction in;
	Direction out;
};

bool running = true;

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

void placeNewFood() {
	int newFoodX, newFoodY;
	do {
		newFoodX = rand() % WIDTH;
		newFoodY = rand() % HEIGHT;
	} while (world[newFoodX][newFoodY].type != EMPTY);
	world[newFoodX][newFoodY].type = FOOD;
}

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

	world = new Tile * [WIDTH];
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

	for (int i = 0; i < FOOD_COUNT; i++) {
		placeNewFood();
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

void handleInputEvents() {
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
}

void gameTick() {
	Snek* nextTail = snekFront;
	do {
		nextTail = nextTail->next;
	} while (nextTail->next->next);
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

	while (!inputQueue.empty() && (inputQueue.front() == snekDirection || (inputQueue.front() + 2) % 4 == snekDirection)) {
		inputQueue.pop();
	}
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

		placeNewFood();
	}

	running = world[snekFront->x][snekFront->y].type != SNEK;
	world[snekFront->x][snekFront->y].type = SNEK;
}

void renderTile(int i, int j) {
	SDL_Rect renderTarget = { i * FIELD, j * FIELD, FIELD, FIELD };
	SDL_Texture* texture = nullptr;
	double angle = 0;
	SDL_RendererFlip flip = SDL_FLIP_NONE;
	switch (world[i][j].type) {
	case EMPTY:
		texture = textures[BACK];
		break;
	case SNEK:
		if (world[i][j].out == NONE) {
			texture = textures[HEAD];
			switch (world[i][j].in) {
			case LEFT:
				flip = SDL_FLIP_HORIZONTAL;
				break;
			case UP:
				angle = 270;
				break;
			case DOWN:
				angle = 90;
				break;
			}
		}

		if (
			(world[i][j].out == UP && world[i][j].in == UP) ||
			(world[i][j].out == DOWN && world[i][j].in == DOWN)
		) {
			texture = textures[BODY];
			angle = 90;
		}
		if (
			(world[i][j].out == LEFT && world[i][j].in == LEFT) ||
			(world[i][j].out == RIGHT && world[i][j].in == RIGHT)
		) {
			texture = textures[BODY];
		}



		if (
			(world[i][j].in == UP && world[i][j].out == RIGHT) ||
			(world[i][j].in == LEFT && world[i][j].out == DOWN)
		) {
			texture = textures[TURN];
		}
		if (
			(world[i][j].in == DOWN && world[i][j].out == RIGHT) ||
			(world[i][j].in == LEFT && world[i][j].out == UP)
		) {
			texture = textures[TURN];
			flip = SDL_FLIP_VERTICAL;
		}
		if ((world[i][j].in == UP && world[i][j].out == LEFT) ||
			(world[i][j].in == RIGHT && world[i][j].out == DOWN)
		) {
			texture = textures[TURN];
			flip = SDL_FLIP_HORIZONTAL;
		}
		if ((world[i][j].in == RIGHT && world[i][j].out == UP) ||
			(world[i][j].in == DOWN && world[i][j].out == LEFT)
		) {
			texture = textures[TURN];
			angle = 180;
		}


		if (world[i][j].in == NONE) {
			texture = textures[TAIL];
			switch (world[i][j].out) {
			case RIGHT:
				break;
			case LEFT:
				flip = SDL_FLIP_HORIZONTAL;
				break;
			case UP:
				angle = 270;
				break;
			case DOWN:
				angle = 90;
				break;
			}
		}

		break;
	case FOOD:
		texture = textures[FOOD_TEXTURE];
		break;
	}
	SDL_RenderCopyEx(renderer, texture, nullptr, &renderTarget, angle, nullptr, flip);
}

int main(int argc, char** argv) {
	init();
	int count = 60;
	while (running) {
		handleInputEvents();

		count--;
		if (!count) {
			count = 15;
			gameTick();
		}

		SDL_RenderClear(renderer);
		for (int i = 0; i < WIDTH; i++) {
			for (int j = 0; j < HEIGHT; j++) {
				renderTile(i, j);
			}
		}
		SDL_RenderPresent(renderer);
	}

	close();
	return 0;
}