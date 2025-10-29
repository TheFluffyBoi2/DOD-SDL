#include "Game.h"
#include <iostream>

Game::Game() {
	if (!SDL_CreateWindowAndRenderer("Dod Project", width, height, 0, &window, &renderer)) {
		SDL_Log(SDL_GetError());
	}

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(renderer);
	SDL_RenderPresent(renderer);

	deltaTime = 0;
	running = true;
}

void Game::Destroy() {
	SDL_Quit();
}

void Game::RunLoop() {
	Uint64 Current{ SDL_GetPerformanceCounter() }, Last;

	SDL_Event currentEvent;

	SpawnAsteroid(width / 2, height / 2, 10);
	SpawnAsteroid(width / 3, height / 3, 20);
	SpawnAsteroid(width / 4, height / 4, 50);
	SpawnAsteroid(width / 5, height / 4, 60);
	SpawnAsteroid(width / 6, height / 4, 70);
	SpawnAsteroid(width / 7, height / 4, 80);

	while (running) {
		Last = Current;
		Current = SDL_GetPerformanceCounter();
		
		deltaTime = (float)((float)(Current - Last) / SDL_GetPerformanceFrequency());
		std::cout << 1 / deltaTime << '\n';

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

void Game::ResolveCollision() {
	for (int i = 0; i < asteroids.size(); i++) {
		for (int j = i+1; j < asteroids.size(); j++) {
			if (i != j) {
				if ((asteroids[i].cx - asteroids[j].cx) *
					(asteroids[i].cx - asteroids[j].cx) +
					(asteroids[i].cy - asteroids[j].cy) *
					(asteroids[i].cy - asteroids[j].cy) <=
					(asteroids[i].r + asteroids[j].r) *
					(asteroids[i].r + asteroids[j].r)) {
					float distance = SDL_sqrtf((asteroids[i].cx - asteroids[j].cx) * (asteroids[i].cx - asteroids[j].cx) +
						(asteroids[i].cy - asteroids[j].cy) * (asteroids[i].cy - asteroids[j].cy));
					if (distance == 0)
						distance = 0.01f;
					float overlap = 0.5f * (distance - asteroids[i].r - asteroids[j].r);

					float nx = (asteroids[i].cx - asteroids[j].cx) / distance;
					float ny = (asteroids[i].cy - asteroids[j].cy) / distance;

					asteroids[i].cx -= overlap * nx;
					asteroids[i].cy -= overlap * ny;

					asteroids[j].cx += overlap * nx;
					asteroids[j].cy += overlap * ny;

					float tx = -ny;
					float ty = nx;

					float dpTan1 = asteroids[i].dx * tx + asteroids[i].dy * ty;
					float dpTan2 = asteroids[j].dx * tx + asteroids[j].dy * ty;

					float dpNorm1 = asteroids[i].dx * nx + asteroids[i].dy * ny;
					float dpNorm2 = asteroids[j].dx * nx + asteroids[j].dy * ny;

					float m1 = (dpNorm1 * (asteroids[i].r * 10.f - asteroids[j].r * 10.f) + 2.0f * asteroids[j].r * 10.f * dpNorm2) / (asteroids[i].r * 10.f + asteroids[j].r * 10.f);
					float m2 = (dpNorm2 * (asteroids[j].r * 10.f - asteroids[i].r * 10.f) + 2.0f * asteroids[i].r * 10.f * dpNorm1) / (asteroids[i].r * 10.f + asteroids[j].r * 10.f);

					asteroids[i].dx = tx * dpTan1 + nx * m1;
					asteroids[i].dy = ty * dpTan1 + ny * m1;
					asteroids[j].dx = tx * dpTan2 + nx * m2;
					asteroids[j].dy = ty * dpTan2 + ny * m2;
				}
			}
		}
	}
}

void Game::Update() {
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(renderer);

	ResolveCollision();
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
