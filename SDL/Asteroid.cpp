#include "Asteroid.h"

Asteroid::Asteroid(float xc, float yc, float r) {
	this->r = r;
	cx = xc;
	cy = yc;

	speed = 300.f;
	
	angle = 360 * SDL_randf();

	dx = SDL_cos(angle) * speed;
	dy = SDL_sin(angle) * speed;
}

void Asteroid::Draw(SDL_Renderer* renderer) {
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);

	float x = r;
	float y = 0;
	
	while (x >= y) {
		SDL_RenderPoint(renderer, cx + x, cy + y);
		SDL_RenderPoint(renderer, cx + x, cy - y);
		SDL_RenderPoint(renderer, cx - x, cy + y);
		SDL_RenderPoint(renderer, cx - x, cy - y);
		SDL_RenderPoint(renderer, cx + y, cy + x);
		SDL_RenderPoint(renderer, cx + y, cy - x);
		SDL_RenderPoint(renderer, cx - y, cy + x);
		SDL_RenderPoint(renderer, cx - y, cy - x);

		y++;
		float xMid = x - 0.5f;

		if (xMid * xMid + y * y >= r * r)
			x--;
	}
}

void Asteroid::Move(float delta) {
	cx += dx * delta;
	cy += dy * delta;
}

void Asteroid::BorderCollision() {
	if (cx + r >= 1280 || cx - r <= 0) {
		dx = -dx;
	}

	if (cy + r >= 720 || cy - r <= 0) {
		dy = -dy;
	}

	cx = SDL_clamp(cx, r, 1280 - r);
	cy = SDL_clamp(cy, r, 720 - r);

}