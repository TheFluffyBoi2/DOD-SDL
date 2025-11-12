#include <SDL3/SDL.h>
#include "Game.h"
#include <vector>
#include <iostream>

void Draw(SDL_Renderer* renderer, std::pair<float, float>& center, float& radius) {
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);

	float x = radius;
	float y = 0;

	while (x >= y) {
		SDL_RenderPoint(renderer, center.first + x, center.second + y);
		SDL_RenderPoint(renderer, center.first + x, center.second - y);
		SDL_RenderPoint(renderer, center.first - x, center.second + y);
		SDL_RenderPoint(renderer, center.first - x, center.second - y);
		SDL_RenderPoint(renderer, center.first + y, center.second + x);
		SDL_RenderPoint(renderer, center.first + y, center.second - x);
		SDL_RenderPoint(renderer, center.first - y, center.second + x);
		SDL_RenderPoint(renderer, center.first - y, center.second - x);

		y++;
		float xMid = x - 0.5f;

		if (xMid * xMid + y * y >= radius * radius)
			x--;
	}
}

void Move(std::pair<float, float>& center, std::pair<float, float>& velocity, float delta) {
	center.first += velocity.first * delta;
	center.second += velocity.second * delta;
}

void BorderCollision(std::pair<float, float>& center, std::pair<float, float>& velocity, float& radius) {
	if (center.first + radius >= 1280 || center.first - radius <= 0) {
		velocity.first = -velocity.first;
	}

	if (center.second + radius >= 720 || center.second - radius <= 0) {
		velocity.second = -velocity.second;
	}

	center.first = SDL_clamp(center.first, radius, 1280 - radius);
	center.second = SDL_clamp(center.second, radius, 720 - radius);

}

void ResolveCollision(int index, std::pair<float, float> centers[], std::pair<float, float> velocities[], float radiuses[]) {
	for (int i = 0; i < index; i++) {
		for (int j = i + 1; j < index; j++) {
			if (i != j) {
				if ((centers[i].first - centers[j].first) *
					(centers[i].first - centers[j].first) +
					(centers[i].second - centers[j].second) *
					(centers[i].second - centers[j].second) <=
					(radiuses[i] + radiuses[j]) *
					(radiuses[i] + radiuses[j])) {
					float distance = SDL_sqrtf((centers[i].first - centers[j].first) * (centers[i].first - centers[j].first) +
						(centers[i].second - centers[j].second) * (centers[i].second - centers[j].second));

					float nx, ny;

					if (distance == 0) {
						distance = 0.1f;
						nx = 1.0f;
						ny = 0.0f;
					}
					else {
						nx = (centers[i].first - centers[j].first) / distance;
						ny = (centers[i].second - centers[j].second) / distance;
					}

					float overlap = 0.5f * (distance - radiuses[i] - radiuses[j]);

					centers[i].first -= overlap * nx;
					centers[i].second -= overlap * ny;

					centers[j].first += overlap * nx;
					centers[j].second += overlap * ny;

					float tx = -ny;
					float ty = nx;

					float dpTan1 = velocities[i].first * tx + velocities[i].second * ty;
					float dpTan2 = velocities[j].first * tx + velocities[j].second * ty;

					float dpNorm1 = velocities[i].first * nx + velocities[i].second * ny;
					float dpNorm2 = velocities[j].first * nx + velocities[j].second * ny;

					float m1 = (dpNorm1 * (radiuses[i] * 10.f - radiuses[j] * 10.f) + 2.0f * radiuses[j] * 10.f * dpNorm2) / (radiuses[i] * 10.f + radiuses[j] * 10.f);
					float m2 = (dpNorm2 * (radiuses[j] * 10.f - radiuses[i] * 10.f) + 2.0f * radiuses[i] * 10.f * dpNorm1) / (radiuses[i] * 10.f + radiuses[j] * 10.f);

					velocities[i].first = tx * dpTan1 + nx * m1;
					velocities[i].second = ty * dpTan1 + ny * m1;
					velocities[j].first = tx * dpTan2 + nx * m2;
					velocities[j].second = ty * dpTan2 + ny * m2;
				}
			}
		}
	}
}

int main(int argc, char* argv[]) {
	std::cout << "Choose mode \n(1)OOP\n(2)DOD\n";
	int option;
	std::cin >> option;
	switch (option) {
	case 1:
	{
		Game game;
		game.RunLoop();
		game.Destroy();
		break;
	}
	case 2:
	{
		float* radiuses = new float[100000];
		float* speeds = new float[100000];
		std::pair<float, float>* centers = new std::pair<float, float>[100000];
		std::pair<float, float>* velocities = new std::pair<float, float>[100000];
		int index = 1;

		radiuses[0] = 10.f;
		speeds[0] = 300.f;
		centers[0] = std::pair<float, float>(1280 / 2, 720 / 2);
		velocities[0] = std::pair<float, float>(SDL_cos(360 * SDL_randf()) * speeds[0], SDL_sin(360 * SDL_randf()) * speeds[0]);

		SDL_Window* window;
		SDL_Renderer* renderer;
		if (!SDL_CreateWindowAndRenderer("Dod Project", 1280, 720, 0, &window, &renderer)) {
			SDL_Log(SDL_GetError());
		}

		SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
		SDL_RenderClear(renderer);
		SDL_RenderPresent(renderer);

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

		ImGui::StyleColorsDark();

		ImGuiStyle& style = ImGui::GetStyle();
		style.ScaleAllSizes(1);
		style.FontScaleDpi = 1;

		ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
		ImGui_ImplSDLRenderer3_Init(renderer);

		Uint64 Current{ SDL_GetPerformanceCounter() }, Last;
		bool showDemoWindow = 1;

		float deltaTime = 0.f;
		bool running = true;
		SDL_Event currentEvent;

		while (running) {
			Last = Current;
			Current = SDL_GetPerformanceCounter();

			deltaTime = (float)((float)(Current - Last) / SDL_GetPerformanceFrequency());

			while (SDL_PollEvent(&currentEvent)) {
				ImGui_ImplSDL3_ProcessEvent(&currentEvent);
				if (currentEvent.type == SDL_EVENT_QUIT)
				{
					SDL_Log("Closed the window");
					running = false;
				}
			}

			ImGui_ImplSDLRenderer3_NewFrame();
			ImGui_ImplSDL3_NewFrame();
			ImGui::NewFrame();

			{
				static int spawnedAsteroids = 0;
				static int erasedAsteroids = 0;
				static int counter = 0;
				static int radius = 1;

				ImGui::Begin("Debug window");

				ImGui::SliderInt("Radius", &radius, 1, 50);
				ImGui::SliderInt("Asteroids", &spawnedAsteroids, 1, 100000 - index);
				ImGui::SliderInt("Erase Count", &erasedAsteroids, 1, index);

				if (ImGui::Button("Spawn")) {
					for (int i = 0; i < spawnedAsteroids; ++i) {
						centers[index] = std::pair<float, float>(1280 / 2, 720 / 2);
						radiuses[index] = radius;
						speeds[index] = 300.f;
						velocities[index] = std::pair<float, float>(SDL_cos(360 * SDL_randf()) * speeds[index], SDL_sin(360 * SDL_randf()) * speeds[index]);

						index++;
					}
				}

				if (ImGui::Button("Erase")) {
					if (index) {
						int eraseCount = std::min(erasedAsteroids, index);
						index -= eraseCount;
					}
				}

				ImGui::Text("Application average %.1f FPS", 1 / deltaTime);
				ImGui::End();
			}


				ResolveCollision(index, centers, velocities, radiuses);
				SDL_RenderClear(renderer);
				ImGui::Render();
				for (int i = 0; i < index; ++i) {
					Move(centers[i], velocities[i], deltaTime);
					BorderCollision(centers[i], velocities[i], radiuses[i]);
					Draw(renderer, centers[i], radiuses[i]);
				}
				SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
				ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
				SDL_RenderPresent(renderer);
		}

		delete[] radiuses;
		delete[] speeds;
		delete[] centers;
		delete[] velocities;
		break;
	}

	default: 
	{
		std::cout << "Not a valid option\n";
		break;
	}

	}
	return 0;
}