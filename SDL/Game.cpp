#include "Game.h"
#include <iostream>

Game::Game() {
	if (!SDL_CreateWindowAndRenderer("Dod Project", width, height, 0, &window, &renderer)) {
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
	Uint64 Current{ SDL_GetPerformanceCounter() }, Last;

	SDL_Event currentEvent;

	SpawnAsteroid(width / 2, height / 2, 100);
	SpawnAsteroid(width / 2, height / 2, 100);
	SpawnAsteroid(width / 2, height / 2, 100);
	SpawnAsteroid(width / 2, height / 2, 100);
	SpawnAsteroid(width / 2, height / 2, 100);
	SpawnAsteroid(width / 2, height / 2, 100);
	SpawnAsteroid(width / 2, height / 2, 100);
	SpawnAsteroid(width / 2, height / 2, 100);
	SpawnAsteroid(width / 2, height / 2, 100);
	SpawnAsteroid(width / 2, height / 2, 100);
	SpawnAsteroid(width / 2, height / 2, 100);
	SpawnAsteroid(width / 2, height / 2, 100);
	SpawnAsteroid(width / 2, height / 2, 100);

	while (running) {
		Last = Current;
		Current = SDL_GetPerformanceCounter();
		
		deltaTime = (float)((float)(Current - Last) / SDL_GetPerformanceFrequency());

		while (SDL_PollEvent(&currentEvent)) {
			if (currentEvent.type == SDL_EVENT_QUIT)
			{
				SDL_Log("Closed the window");
				running = false;
			}
		}
		Update();
	}
}

void Game::Update() {
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(renderer);

	for (Asteroid& asteroid : asteroids) {
		asteroid.Move(deltaTime);
		asteroid.Draw(renderer);
		asteroid.BorderCollision();
	}
	SDL_RenderPresent(renderer);
}

void Game::SpawnAsteroid(float xc, float yc, float r) {
	Asteroid asteroid(xc, yc, r);
	asteroids.push_back(asteroid);
}
