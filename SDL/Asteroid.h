#pragma once
#include <SDL3/SDL.h>
#include "Vec2.h"

class Asteroid
{
private:
	float r, speed, angle;
	Vec2 direction, center;

public:
	Asteroid(float xc, float yc, float r);
	void Draw(SDL_Renderer* renderer);
	void Move(float delta);
	void BorderCollision();
};
