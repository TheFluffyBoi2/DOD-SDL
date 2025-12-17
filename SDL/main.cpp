#include "Game.h"
#include "PerformanceCounter.h"
#include "Constants.hpp"
#include <SDL3/SDL.h>
#include <box2d/collision.h>
#include <box2d/box2d.h>
#include <vector>
#include <fstream>
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

//void Draw(SDL_Renderer* renderer, b2Vec2 center, float radius) {
//	SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
//
//	float x = radius;
//	float y = 0;
//
//	while (x >= y) {
//		SDL_RenderPoint(renderer, center.x + x, center.y + y);
//		SDL_RenderPoint(renderer, center.x + x, center.y - y);
//		SDL_RenderPoint(renderer, center.x - x, center.y + y);
//		SDL_RenderPoint(renderer, center.x - x, center.y - y);
//		SDL_RenderPoint(renderer, center.x + y, center.y + x);
//		SDL_RenderPoint(renderer, center.x + y, center.y - x);
//		SDL_RenderPoint(renderer, center.x - y, center.y + x);
//		SDL_RenderPoint(renderer, center.x - y, center.y - x);
//
//		y++;
//		float xMid = x - 0.5f;
//
//		if (xMid * xMid + y * y >= radius * radius)
//			x--;
//	}
//}

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

float rsqrt(float number) {
	int32_t i;
	float x2, y;
	
	x2 = number * 0.5f;
	y = number;
	i = *(int32_t*)&y;
	i = 0x5f3759df - (i >> 1);
	y = *(float*)&i;
	y = y * (1.5f - (x2 * y * y));
	return y;
}

void ResolveCollision(int grid[], int startPos, std::pair<float, float> centers[], std::pair<float, float> velocities[], float radiuses[]) {
	//PerformanceCounter pc{ "Collision" };
	int lastIndex = grid[startPos];
	for (int i = startPos + 1; i < lastIndex; i++) {
		for (int j = i + 1; j < lastIndex; j++) {
			int first_index = grid[i];
			int second_index = grid[j];
			if (first_index != second_index) {
				if ((centers[first_index].first - centers[second_index].first) *
					(centers[first_index].first - centers[second_index].first) +
					(centers[first_index].second - centers[second_index].second) *
					(centers[first_index].second - centers[second_index].second) <=
					(radiuses[first_index] + radiuses[second_index]) *
					(radiuses[first_index] + radiuses[second_index])) {

					//pc.Start();

					float dx = centers[first_index].first - centers[second_index].first;
					float dy = centers[first_index].second - centers[second_index].second;
					float dist2 = dx * dx + dy * dy;


					//float distance = SDL_sqrtf(dist2);

					float nx, ny;
					float distance;

					if (dist2 == 0.0f) {
						distance = 0.1f;
						nx = 1.0f;
						ny = 0.0f;
					}
					else {
						float reverse_dist = rsqrt(dist2);
						distance = dist2 * reverse_dist;
						nx = (centers[first_index].first - centers[second_index].first) * reverse_dist;
						ny = (centers[first_index].second - centers[second_index].second) * reverse_dist;
					}

					float overlap = 0.5f * (distance - radiuses[first_index] - radiuses[second_index]);

					centers[first_index].first -= overlap * nx;
					centers[first_index].second -= overlap * ny;

					centers[second_index].first += overlap * nx;
					centers[second_index].second += overlap * ny;

					//float tx = -ny;
					//float ty = nx;

					//float dpTan1 = velocities[first_index].first * tx + velocities[first_index].second * ty;
					//float dpTan2 = velocities[second_index].first * tx + velocities[second_index].second * ty;

					float dpNorm1 = velocities[first_index].first * nx + velocities[first_index].second * ny;
					float dpNorm2 = velocities[second_index].first * nx + velocities[second_index].second * ny;

					//float m1 = (dpNorm1 * (radiuses[first_index] * 10.f - radiuses[second_index] * 10.f) + 2.0f * radiuses[second_index] * 10.f * dpNorm2) 
					//	/ (radiuses[first_index] * 10.f + radiuses[second_index] * 10.f);
					//float m2 = (dpNorm2 * (radiuses[second_index] * 10.f - radiuses[first_index] * 10.f) + 2.0f * radiuses[first_index] * 10.f * dpNorm1) 
					//	/ (radiuses[first_index] * 10.f + radiuses[second_index] * 10.f);

					//velocities[first_index].first = tx * dpTan1 + nx * m1;
					//velocities[first_index].second = ty * dpTan1 + ny * m1;
					//velocities[second_index].first = tx * dpTan2 + nx * m2;
					//velocities[second_index].second = ty * dpTan2 + ny * m2;

					velocities[first_index].first += (dpNorm2 - dpNorm1) * nx;
					velocities[first_index].second += (dpNorm2 - dpNorm1) * ny;
					velocities[second_index].first -= (dpNorm2 - dpNorm1) * nx;
					velocities[second_index].second -= (dpNorm2 - dpNorm1) * ny;

					//pc.End();
				}
			}
		}
	}

	//pc.PrintAverage();
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

struct s_Asteroid {
	int radius;
	float speed;
	std::pair<float, float> center;
	std::pair<float, float> velocity;
};

void InsertAsteroid(int grid[], int* index, std::pair<float, float>* position) {
	int xPos = (*position).first / Constants::CELL_WIDTH;
	int yPos = (*position).second / Constants::CELL_WIDTH;
	
	int posIndex = yPos * Constants::GRID_LINE_SIZE + xPos * Constants::CELL_SIZE;
	if (grid[posIndex] - posIndex - 1 >= 400) {
	}
	else {
		int freePos = grid[posIndex];
		grid[freePos] = *index;
		grid[posIndex]++;
	}
}

void UpdateCell(int grid[], int startPos, std::pair<float, float> centers[]) {
	for (int i = startPos + 1; i < grid[startPos];) {
		int index = grid[i];
		int xPos = centers[index].first / Constants::CELL_WIDTH;
		int yPos = centers[index].second / Constants::CELL_WIDTH;

		int posIndex = yPos * Constants::GRID_LINE_SIZE + xPos * Constants::CELL_SIZE;

		if (posIndex != startPos) {
			if (grid[posIndex] - posIndex - 1 >= 400) {
				i++;
				continue;
			}
			std::swap(grid[i], grid[grid[startPos] - 1]);
			int freePos = grid[posIndex];
			grid[freePos] = grid[i];
			grid[posIndex]++;
			grid[startPos]--;
		}
		else {
			i++;
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
		// BOX2D TEST
		//b2WorldDef worldDef = b2DefaultWorldDef();
		//worldDef.restitutionThreshold = 10.0f;
		//b2WorldId worldId = b2CreateWorld(&worldDef);

		//b2BodyDef bodyDef = b2DefaultBodyDef();
		//bodyDef.type = b2_dynamicBody;
		//bodyDef.position = b2Vec2{ 640.0f, 360.0f };
		//bodyDef.linearVelocity = b2Vec2{ 100.0f, 100.0f };
		//bodyDef.gravityScale = 0.0f;
		//b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);
		//bodyDef.linearVelocity = b2Vec2{ -100.0f, -100.0f };
		//bodyDef.position = b2Vec2{ 740.0f, 460.0f };
		//b2BodyId bodyId2 = b2CreateBody(worldId, &bodyDef);

		//b2Circle circle = { b2Vec2{0.0f, 0.0f}, 50.0f };
		//b2ShapeDef circleShapeDef = b2DefaultShapeDef();
		//circleShapeDef.density = 1.0f;
		//b2ShapeId circle1 = b2CreateCircleShape(bodyId, &circleShapeDef, &circle);
		//b2ShapeId circle2 = b2CreateCircleShape(bodyId2, &circleShapeDef, &circle);
		//b2Shape_SetRestitution(circle1, 1.0f);
		//b2Shape_SetRestitution(circle1, 1.0f);
		//int subStepCount = 4;
		//float timeStep{1/60.f};

		//SDL_Window* window;
		//SDL_Renderer* renderer;
		//if (!SDL_CreateWindowAndRenderer("Dod Project", 1280, 720, 0, &window, &renderer)) {
		//	SDL_Log(SDL_GetError());
		//}

		//SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
		//SDL_RenderClear(renderer);
		//SDL_RenderPresent(renderer);

		//Uint64 Current{ SDL_GetPerformanceCounter() }, Last;
		//float deltaTime = 0.0f;
		//double accumulator = 0.0f;
		//bool running = true;
		//SDL_Event currentEvent;

		//while (running) {
		//	Last = Current;
		//	Current = SDL_GetPerformanceCounter();

		//	deltaTime = (float)((float)(Current - Last) / SDL_GetPerformanceFrequency());
		//	accumulator += deltaTime;
		//	//timeStep = deltaTime;


		//	while (accumulator >= timeStep) {
		//		b2World_Step(worldId, timeStep, subStepCount);
		//		b2Vec2 position = b2Body_GetPosition(bodyId);
		//		b2Rot rotation = b2Body_GetRotation(bodyId);
		//		position = b2Body_GetPosition(bodyId2);
		//		rotation = b2Body_GetRotation(bodyId2);
		//		accumulator -= timeStep;
		//	}


		//	while (SDL_PollEvent(&currentEvent)) {
		//		if (currentEvent.type == SDL_EVENT_QUIT)
		//		{
		//			SDL_Log("Closed the window");
		//			running = false;
		//		}
		//	}

		//	SDL_RenderClear(renderer);
		//	Draw(renderer, b2Body_GetPosition(bodyId), 50.f);
		//	Draw(renderer, b2Body_GetPosition(bodyId2), 50.f);
		//	SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
		//	SDL_RenderPresent(renderer);
		//}
		
		// *********************************************************************************************

		// HAND-WRITTEN PHYSICS
		float* radiuses = new float[Constants::MAX_ASTEROIDS];
		float* speeds = new float[Constants::MAX_ASTEROIDS];
		std::pair<float, float>* centers = new std::pair<float, float>[Constants::MAX_ASTEROIDS];
		std::pair<float, float>* velocities = new std::pair<float, float>[Constants::MAX_ASTEROIDS];
		int index = 0;

		int* grid = new int[Constants::GRID_SIZE];
		int* gridSlice = new int[Constants::ADJOINED_CELL_SIZE];
		int size = 0;

		for (int i = 0; i < Constants::GRID_SIZE; i += Constants::CELL_SIZE) {
			grid[i] = i + 1;
		}

		radiuses[index] = 10.f;
		speeds[index] = 50.f;
		centers[index] = std::pair<float, float>(Constants::WIDTH / 2, Constants::HEIGHT / 2);
		velocities[index] = std::pair<float, float>(SDL_cos(360 * SDL_randf()) * speeds[0], SDL_sin(360 * SDL_randf()) * speeds[0]);
		InsertAsteroid(grid, &index, &centers[index]);
		index++;

		SDL_Window* window;
		SDL_Renderer* renderer;
		float viewportScale = 1.0f;

		if (!SDL_CreateWindowAndRenderer("Dod Project", Constants::WIDTH, Constants::HEIGHT, 0, &window, &renderer)) {
			SDL_Log(SDL_GetError());
		}

		SDL_SetRenderScale(renderer, viewportScale, viewportScale);

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

				ImGui::SliderInt("Radius", &radius, 1, 10);
				ImGui::SliderInt("Asteroids", &spawnedAsteroids, 1, 100'000);
				ImGui::SliderInt("Erase Count", &erasedAsteroids, 1, index);

				if (ImGui::Button("Spawn")) {
					for (int i = 0; i < spawnedAsteroids; ++i) {
						centers[index] = std::pair<float, float>(SDL_rand(Constants::WIDTH),  SDL_rand(Constants::HEIGHT));
						radiuses[index] = radius;
						speeds[index] = 50.f;
						velocities[index] = std::pair<float, float>(SDL_cos(360 * SDL_randf()) * speeds[index], SDL_sin(360 * SDL_randf()) * speeds[index]);
						InsertAsteroid(grid, &index, &centers[index]);
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

			for (int i = 0; i < Constants::CELL_COUNT; i++) {
				UpdateCell(grid, Constants::CELL_SIZE * i, centers);
			}

			for (int i = 0; i < Constants::CELL_COUNT; i++) {
				    
				ResolveCollision(grid, Constants::CELL_SIZE * i, centers, velocities, radiuses);
			}
			//ResolveCollision(index, centers, velocities, radiuses);
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
		delete[] gridSlice;
		delete[] grid;
		break;
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