#pragma once
#include <SDL3/SDL.h>
#include <vector>
#include "Asteroid.h"

class Game
{
private:
	bool running{ false };
	int32_t width{ 1280 }, height{ 720 };
	float deltaTime;
	SDL_Window* window;
	SDL_Renderer* renderer;
	std::vector<Asteroid> asteroids;

public:
	Game();
	void Destroy();
	void RunLoop();
	void Update();
	void SpawnAsteroid(float xc, float yc, float r);
};
