#include <SDL3/SDL.h>
#include "Game.h"

int main(int argc, char* argv[]) {
	Game game;
	game.RunLoop();
	game.Destroy(); 
	return 0;
}
