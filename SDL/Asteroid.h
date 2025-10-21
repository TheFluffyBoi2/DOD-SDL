#pragma once
#include <SDL3/SDL.h>

class Asteroid
{
private:
	float xc, yc, r;

public:
	Asteroid(float xc, float yc, float r);
	void Draw(SDL_Renderer* renderer) const;
};
