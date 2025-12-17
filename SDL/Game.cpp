#include "Game.h"

Game::Game() {
	if (!SDL_CreateWindowAndRenderer("Dod Project", 1280, 720, 0, &window, &renderer)) {
		SDL_Log(SDL_GetError());
	}

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(renderer);
	SDL_RenderPresent(renderer);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

	ImGui::StyleColorsDark();
	
	ImGuiStyle& style = ImGui::GetStyle();
	style.ScaleAllSizes(1);
	style.FontScaleDpi = 1;

	ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
	ImGui_ImplSDLRenderer3_Init(renderer);

	deltaTime = 0;
	running = true;
}

void Game::Destroy() {
	SDL_Quit();
}

void Game::RunLoop() {
	Uint64 Current{ SDL_GetPerformanceCounter() }, Last;
	bool showDemoWindow = 1;

	SDL_Event currentEvent;

	SpawnAsteroid(1280 / 2, 720 / 2, 10);

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
			ImGui::SliderInt("Asteroids", &spawnedAsteroids, 1, 50000 - asteroids.size());
			ImGui::SliderInt("Erase Count", &erasedAsteroids, 1, asteroids.size());

			if (ImGui::Button("Spawn")) {
				for (int i = 0; i < spawnedAsteroids; ++i)
					SpawnAsteroid(1280 / 2, 720 / 2, radius);
			}

			if (ImGui::Button("Erase")) {
				if (!asteroids.empty()) {
					int eraseCount = std::min(erasedAsteroids, (int)asteroids.size());
					asteroids.erase(asteroids.begin(), asteroids.begin() + eraseCount);
				}
			}

			ImGui::Text("Application average %.1f FPS", 1 / deltaTime);
			ImGui::End();
		}
		Update();
	}

	End();
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
					
					float nx, ny;

					if (distance == 0) {
						distance = 0.1f;
						nx = 1.0f;
						ny = 0.0f;
					}
					else {
						nx = (asteroids[i].cx - asteroids[j].cx) / distance;
						ny = (asteroids[i].cy - asteroids[j].cy) / distance;
					}

;					float overlap = 0.5f * (distance - asteroids[i].r - asteroids[j].r);

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
	SDL_RenderClear(renderer);
	ResolveCollision();
	for (Asteroid& asteroid : asteroids) {
		asteroid.Move(deltaTime);
		asteroid.Draw(renderer);
		asteroid.BorderCollision();
	}

	ImGui::Render();
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
	ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
	SDL_RenderPresent(renderer);
}

void Game::SpawnAsteroid(float xc, float yc, float r) {
	Asteroid asteroid(xc, yc, r);
	asteroids.push_back(asteroid);
}

void Game::End() {
	ImGui_ImplSDLRenderer3_Shutdown();
	ImGui_ImplSDL3_Shutdown();
	ImGui::DestroyContext();

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}
