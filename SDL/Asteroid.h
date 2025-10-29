#pragma once
#include <SDL3/SDL.h>

class Asteroid
{
public:
	float r, speed, angle;
	float dx, dy, cx, cy;
	Asteroid(float xc, float yc, float r);
	void Draw(SDL_Renderer* renderer);
	void Move(float delta);
	void BorderCollision();
};
