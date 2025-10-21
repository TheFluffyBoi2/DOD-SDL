#include "Game.h"

Game::Game() {
	if (!SDL_CreateWindowAndRenderer("Dod Project", width, height, SDL_WINDOW_RESIZABLE, &window, &renderer)) {
		SDL_Log(SDL_GetError());
	}

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(renderer);
	SDL_RenderPresent(renderer);

	running = true;
}

void Game::Destroy() {
	SDL_Quit();
}

void Game::RunLoop() {
	SpawnAsteroid(width / 2, height / 2, 50);

	while (running) {
		SDL_Event currentEvent;
		while (SDL_PollEvent(&currentEvent)) {
			if (currentEvent.type == SDL_EVENT_QUIT)
			{
				SDL_Log("Closed the window");
				running = false;
			}
		}

		SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
		SDL_RenderClear(renderer);
		
		for (const Asteroid& asteroid : asteroids) {
			asteroid.Draw(renderer);
		}
		SDL_RenderPresent(renderer);
	}
}

void Game::Update() {
	//Work in progress
}

void Game::SpawnAsteroid(float xc, float yc, float r) {
	Asteroid asteroid(xc, yc, r);
	asteroids.push_back(asteroid);
}
