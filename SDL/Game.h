#pragma once
#include <SDL3/SDL.h>
#include <iostream>
#include <vector>
#include "Asteroid.h"
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"

class Game
{
private:
	bool running{ false };
	float deltaTime;
	SDL_Window* window;
	SDL_Renderer* renderer;
	ImGuiIO io;
	std::vector<Asteroid> asteroids;
	std::vector<std::pair<int, int>> overlapAsteroids;

public:
	Game();
	void Destroy();
	void RunLoop();
	void Update();
	void End();
	void SpawnAsteroid(float xc, float yc, float r);
	void ResolveCollision();
};
