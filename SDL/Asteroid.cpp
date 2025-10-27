#include "Asteroid.h"
#include <iostream>

Asteroid::Asteroid(float xc, float yc, float r) {
	center = Vec2(xc, yc);

	this->r = r;

	speed = 100.f;
	
	angle = 360 * SDL_randf();

	float dirx = SDL_cos(angle);
	float diry = SDL_sin(angle);

	direction = Vec2(dirx * speed, diry * speed);
}

void Asteroid::Draw(SDL_Renderer* renderer) {
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);

	float x = r;
	float y = 0;
	
	while (x >= y) {
		SDL_RenderPoint(renderer, center.x + x, center.y + y);
		SDL_RenderPoint(renderer, center.x + x, center.y - y);
		SDL_RenderPoint(renderer, center.x - x, center.y + y);
		SDL_RenderPoint(renderer, center.x - x, center.y - y);
		SDL_RenderPoint(renderer, center.x + y, center.y + x);
		SDL_RenderPoint(renderer, center.x + y, center.y - x);
		SDL_RenderPoint(renderer, center.x - y, center.y + x);
		SDL_RenderPoint(renderer, center.x - y, center.y - x);

		y++;
		float xMid = x - 0.5f;

		if (xMid * xMid + y * y >= r * r)
			x--;
	}
}

void Asteroid::Move(float delta) {
	center.Addvf2(direction.Multiplyf(delta));
}

void Asteroid::BorderCollision() {
	if (center.x + r >= 1280 || center.x - r <= 0) {
		direction.x = -direction.x;
	}

	if (center.y + r >= 720 || center.y - r <= 0) {
		direction.y = -direction.y;
	}
}