#include <iostream>

#include <SDL.h>

using namespace std;

int main(int argc, char** argv) {
	SDL_Init(SDL_INIT_VIDEO);
	cout << "Hello CMake." << endl;
	return 0;
	SDL_Quit();
}
