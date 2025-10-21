#include "Asteroid.h"
#include <iostream>

Asteroid::Asteroid(float xc, float yc, float r) {
	this->xc = xc;
	this->yc = yc;
	this->r = r;
}

void Asteroid::Draw(SDL_Renderer* renderer) const {
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);

	float x = r;
	float y = 0;
	
	while (x >= y) {
		SDL_RenderPoint(renderer, xc + x, yc + y);
		SDL_RenderPoint(renderer, xc + x, yc - y);
		SDL_RenderPoint(renderer, xc - x, yc + y);
		SDL_RenderPoint(renderer, xc - x, yc - y);
		SDL_RenderPoint(renderer, xc + y, yc + x);
		SDL_RenderPoint(renderer, xc + y, yc - x);
		SDL_RenderPoint(renderer, xc - y, yc + x);
		SDL_RenderPoint(renderer, xc - y, yc - x);

		y++;
		float xMid = x - 0.5f;

		if (xMid * xMid + y * y >= r * r)
			x--;
	}
}
