#include <SDL3/SDL.h>
//#include <box2d/box2d.h>
//#include <box2d/collision.h>

#include <iostream>
#include <omp.h>

#include "Constants.hpp"
#include "Game.h"
#include "PerformanceCounter.h"

struct vec2f {
	float x;
	float y;
};

struct vec2i {
	int x;
	int y;
};

void Draw(SDL_Renderer* renderer, vec2f center, float radius) {
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);

	float x = radius;
	float y = 0;

	while (x >= y) {
		SDL_FPoint points[8] = {
			{center.x + x, center.y + y}, {center.x + x, center.y - y},
			{center.x - x, center.y + y}, {center.x - x, center.y - y},
			{center.x + y, center.y + x}, {center.x + y, center.y - x},
			{center.x - y, center.y + x}, {center.x - y, center.y - x} };

		SDL_RenderPoints(renderer, points, 8);

		y++;
		float xMid = x - 0.5f;

		if (xMid * xMid + y * y >= radius * radius)
			x--;
	}
}

// void Draw(SDL_Renderer* renderer, b2Vec2 center, float radius) {
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
// }

void Move(vec2f& center, vec2f velocity, float delta) {
	center.x += velocity.x * delta;
	center.y += velocity.y * delta;
}

void BorderCollision(vec2f& center, vec2f& velocity, float radius) {
	if (center.x + radius >= 1280 || center.x - radius <= 0) {
		velocity.x = -velocity.x;
	}

	if (center.y + radius >= 720 || center.y - radius <= 0) {
		velocity.y = -velocity.y;
	}

	center.x = SDL_clamp(center.x, radius, 1280 - radius);
	center.y = SDL_clamp(center.y, radius, 720 - radius);
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

void AddCollision(vec2i collisions[], int& col_nr, int grid[], int startPos, vec2f centers[], float radiuses[]) {
	int lastIndex = grid[startPos];

	for (int i = startPos + 1; i < lastIndex; i++) {
		for (int j = i + 1; j < lastIndex; j++) {
			int first_index = grid[i];
			int second_index = grid[j];
			if (first_index < second_index) {
				if ((centers[first_index].x - centers[second_index].x) *
					(centers[first_index].x - centers[second_index].x) +
					(centers[first_index].y - centers[second_index].y) *
					(centers[first_index].y - centers[second_index].y) <=
					(radiuses[first_index] + radiuses[second_index]) *
					(radiuses[first_index] + radiuses[second_index]))
					#pragma omp critical
					{
					collisions[col_nr] = vec2i{ first_index, second_index };
					col_nr++;
					}
			}
		}
	}
}

void ResolveCollision(vec2i collisions[], int col_nr, vec2f centers[], float radiuses[], vec2f velocities[]) {
	for (int i = 0; i < col_nr; i++) {
		int first_index = collisions[i].x;
		int second_index = collisions[i].y;

		float dx = centers[first_index].x - centers[second_index].x;
		float dy = centers[first_index].y - centers[second_index].y;
		float dist2 = dx * dx + dy * dy;

		float nx, ny;
		float distance;

		if (dist2 == 0.0f) {
			distance = 0.1f;
			nx = 1.0f;
			ny = 0.0f;
		} else {
			float reverse_dist = rsqrt(dist2);
			distance = dist2 * reverse_dist;
			nx = (centers[first_index].x - centers[second_index].x) *
				reverse_dist;
			ny = (centers[first_index].y - centers[second_index].y) *
				reverse_dist;
		}

		float overlap = 0.5f * (distance - radiuses[first_index] -
			radiuses[second_index]);

		centers[first_index].x -= overlap * nx;
		centers[first_index].y -= overlap * ny;

		centers[second_index].x += overlap * nx;
		centers[second_index].y += overlap * ny;
		
		float dpNorm1 =
			velocities[first_index].x * nx + velocities[first_index].y * ny;
		float dpNorm2 =
			velocities[second_index].x * nx + velocities[second_index].y * ny;

		velocities[first_index].x += (dpNorm2 - dpNorm1) * nx;
		velocities[first_index].y += (dpNorm2 - dpNorm1) * ny;
		velocities[second_index].x -= (dpNorm2 - dpNorm1) * nx;
		velocities[second_index].y -= (dpNorm2 - dpNorm1) * ny;
	}
}

void ResolveCollision(int grid[], int startPos, vec2f centers[],
	vec2f velocities[], float radiuses[]) {
	int lastIndex = grid[startPos];

	for (int i = startPos + 1; i < lastIndex; i++) {
		for (int j = i + 1; j < lastIndex; j++) {
			int first_index = grid[i];
			int second_index = grid[j];
			if (first_index < second_index) {
				if ((centers[first_index].x - centers[second_index].x) *
					(centers[first_index].x - centers[second_index].x) +
					(centers[first_index].y - centers[second_index].y) *
					(centers[first_index].y - centers[second_index].y) <=
					(radiuses[first_index] + radiuses[second_index]) *
					(radiuses[first_index] + radiuses[second_index])) {
					float dx = centers[first_index].x - centers[second_index].x;
					float dy = centers[first_index].y - centers[second_index].y;
					float dist2 = dx * dx + dy * dy;

					// float distance = SDL_sqrtf(dist2);

					float nx, ny;
					float distance;

					if (dist2 == 0.0f) {
						distance = 0.1f;
						nx = 1.0f;
						ny = 0.0f;
					} else {
						float reverse_dist = rsqrt(dist2);
						distance = dist2 * reverse_dist;
						nx = (centers[first_index].x - centers[second_index].x) *
							reverse_dist;
						ny = (centers[first_index].y - centers[second_index].y) *
							reverse_dist;
					}

					float overlap = 0.5f * (distance - radiuses[first_index] -
						radiuses[second_index]);

					centers[first_index].x -= overlap * nx;
					centers[first_index].y -= overlap * ny;

					centers[second_index].x += overlap * nx;
					centers[second_index].y += overlap * ny;

					// float tx = -ny;
					// float ty = nx;

					// float dpTan1 = velocities[first_index].first * tx +
					// velocities[first_index].second * ty; float dpTan2 =
					// velocities[second_index].first * tx +
					// velocities[second_index].second * ty;

					float dpNorm1 =
						velocities[first_index].x * nx + velocities[first_index].y * ny;
					float dpNorm2 =
						velocities[second_index].x * nx + velocities[second_index].y * ny;

					// float m1 = (dpNorm1 * (radiuses[first_index] * 10.f -
					// radiuses[second_index] * 10.f) + 2.0f * radiuses[second_index]
					// * 10.f * dpNorm2) 	/ (radiuses[first_index] * 10.f +
					// radiuses[second_index] * 10.f); float m2 = (dpNorm2 *
					// (radiuses[second_index] * 10.f - radiuses[first_index] * 10.f)
					// + 2.0f * radiuses[first_index] * 10.f * dpNorm1) 	/
					//(radiuses[first_index] * 10.f + radiuses[second_index] * 10.f);

					// velocities[first_index].first = tx * dpTan1 + nx * m1;
					// velocities[first_index].second = ty * dpTan1 + ny * m1;
					// velocities[second_index].first = tx * dpTan2 + nx * m2;
					// velocities[second_index].second = ty * dpTan2 + ny * m2;

					velocities[first_index].x += (dpNorm2 - dpNorm1) * nx;
					velocities[first_index].y += (dpNorm2 - dpNorm1) * ny;
					velocities[second_index].x -= (dpNorm2 - dpNorm1) * nx;
					velocities[second_index].y -= (dpNorm2 - dpNorm1) * ny;
				}
			}
		}
	}
}

void InsertAsteroid(int grid[], int index, vec2f position, float radius) {
	int x = position.x;
	int y = position.y;

	float invCell = 1.0f / Constants::CELL_WIDTH;

	int cellX = (int)(x * invCell);
	int cellY = (int)(y * invCell);
	int cellXMin = std::max((int)((x - radius) * invCell), 0);
	int cellXMax = std::min((int)((x + radius) * invCell), Constants::MAX_CELL_X);
	int cellYMin = std::max((int)((y - radius) * invCell), 0);
	int cellYMax = std::min((int)((y + radius) * invCell), Constants::MAX_CELL_Y);

	vec2i gridPos[9] = {
		vec2i { cellX, cellY },
		vec2i { cellX, cellYMax },
		vec2i { cellX, cellYMin },
		vec2i { cellXMin, cellY },
		vec2i { cellXMax, cellY },
		vec2i { cellXMin, cellYMax },
		vec2i { cellXMax, cellYMax },
		vec2i { cellXMin, cellYMin },
		vec2i { cellXMax, cellYMin },
	};

	for (int i = 0; i < 9; i++) {
		bool duplicate = false;
		for (int j = 0; j < i; j++)
			if (gridPos[i].x == gridPos[j].x && gridPos[i].y == gridPos[j].y) {
				duplicate = true;
				break;
			}
		if (!duplicate) {
			int posIndex = gridPos[i].y * Constants::GRID_LINE_SIZE +
				gridPos[i].x * Constants::CELL_SIZE;
			if (grid[posIndex] - posIndex - 1 >= 400) {
				 std::cout << "UH OH";
			} else {
				int freePos = SDL_AddAtomicInt((SDL_AtomicInt*)&grid[posIndex], 1);
				grid[freePos] = index;
			}
		}
	}
}

void UpdateGrid(int grid[], vec2f centers[], int index, float radiuses[]) {
	#pragma omp parallel for
	for (int i = 0; i < Constants::GRID_SIZE; i += Constants::CELL_SIZE)
		grid[i] = i + 1;

	#pragma omp parallel for
	for (int i = 0; i < index; i++)
		InsertAsteroid(grid, i, centers[i], radiuses[i]);
}

// void UpdateCell(int grid[], int startPos, std::pair<float, float> centers[])
// { 	for (int i = startPos + 1; i < grid[startPos];) { 		int
// index = grid[i]; 		int
// xPos = centers[index].first / Constants::CELL_WIDTH; 		int yPos
// = centers[index].second / Constants::CELL_WIDTH;
//
//		int posIndex = yPos * Constants::GRID_LINE_SIZE + xPos *
// Constants::CELL_SIZE;
//
//		if (posIndex != startPos) {
//			if (grid[posIndex] - posIndex - 1 >= 400) {
//				i++;
//				continue;
//			}
//			std::swap(grid[i], grid[grid[startPos] - 1]);
//			int freePos = grid[posIndex];
//			grid[freePos] = grid[i];
//			grid[posIndex]++;
//			grid[startPos]--;
//		} else
//			i++;
//	}
// }

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
		// b2WorldDef worldDef = b2DefaultWorldDef();
		// worldDef.restitutionThreshold = 10.0f;
		// b2WorldId worldId = b2CreateWorld(&worldDef);

		// b2BodyDef bodyDef = b2DefaultBodyDef();
		// bodyDef.type = b2_dynamicBody;
		// bodyDef.position = b2Vec2{ 640.0f, 360.0f };
		// bodyDef.linearVelocity = b2Vec2{ 100.0f, 100.0f };
		// bodyDef.gravityScale = 0.0f;
		// b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);
		// bodyDef.linearVelocity = b2Vec2{ -100.0f, -100.0f };
		// bodyDef.position = b2Vec2{ 740.0f, 460.0f };
		// b2BodyId bodyId2 = b2CreateBody(worldId, &bodyDef);

		// b2Circle circle = { b2Vec2{0.0f, 0.0f}, 50.0f };
		// b2ShapeDef circleShapeDef = b2DefaultShapeDef();
		// circleShapeDef.density = 1.0f;
		// b2ShapeId circle1 = b2CreateCircleShape(bodyId, &circleShapeDef,
		// &circle); b2ShapeId circle2 = b2CreateCircleShape(bodyId2,
		// &circleShapeDef, &circle); b2Shape_SetRestitution(circle1, 1.0f);
		// b2Shape_SetRestitution(circle1, 1.0f);
		// int subStepCount = 4;
		// float timeStep{1/60.f};

		// SDL_Window* window;
		// SDL_Renderer* renderer;
		// if (!SDL_CreateWindowAndRenderer("Dod Project", 1280, 720, 0, &window,
		// &renderer)) { 	SDL_Log(SDL_GetError());
		// }

		// SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
		// SDL_RenderClear(renderer);
		// SDL_RenderPresent(renderer);

		// Uint64 Current{ SDL_GetPerformanceCounter() }, Last;
		// float deltaTime = 0.0f;
		// double accumulator = 0.0f;
		// bool running = true;
		// SDL_Event currentEvent;

		// while (running) {
		//	Last = Current;
		//	Current = SDL_GetPerformanceCounter();

		//	deltaTime = (float)((float)(Current - Last) /
		// SDL_GetPerformanceFrequency()); 	accumulator += deltaTime;
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
		//	SDL_SetRenderDrawColor(renderer, 255, 255, 255,
		// SDL_ALPHA_OPAQUE); 	SDL_RenderPresent(renderer);
		//}

		// *********************************************************************************************

		// HAND-WRITTEN PHYSICS
		static int spawnedAsteroids = 0;
		static int erasedAsteroids = 0;
		static int counter = 0;
		static int radius = 1;
		static float color[3] = { 1,1,1 };

		float* radiuses = new float[Constants::MAX_ASTEROIDS];
		float* speeds = new float[Constants::MAX_ASTEROIDS];

		vec2f* centers = new vec2f[Constants::MAX_ASTEROIDS];
		vec2f* velocities = new vec2f[Constants::MAX_ASTEROIDS];

		vec2i* collisions = new vec2i[Constants::MAX_ASTEROIDS * 4];

		SDL_Vertex* vertices = new SDL_Vertex[Constants::MAX_ASTEROIDS * 4];
		int* indices = new int[Constants::MAX_ASTEROIDS * 6];

		#pragma omp parallel for
		for (int i = 0; i < Constants::MAX_ASTEROIDS; i++) {
			int indices_index = 6 * i;
			int vertex_index = 4 * i;

			indices[indices_index + 0] = vertex_index + 0;
			indices[indices_index + 1] = vertex_index + 1;
			indices[indices_index + 2] = vertex_index + 2;
			indices[indices_index + 3] = vertex_index + 2;
			indices[indices_index + 4] = vertex_index + 3;
			indices[indices_index + 5] = vertex_index + 0;
		}

		int index = 0;

		int* grid = new int[Constants::GRID_SIZE];
		int* gridSlice = new int[Constants::ADJOINED_CELL_SIZE];

		for (int i = 0; i < Constants::GRID_SIZE; i += Constants::CELL_SIZE)
			grid[i] = i + 1;

		radiuses[index] = 1.f;
		speeds[index] = 50.f;
		centers[index] = vec2f{ Constants::WIDTH / 2, Constants::HEIGHT / 2 };
		velocities[index] = vec2f{ (float)SDL_cos(360 * SDL_randf()) * speeds[0],
								 (float)SDL_sin(360 * SDL_randf()) * speeds[0] };
		InsertAsteroid(grid, index, centers[index], radiuses[index]);
		index++;

		SDL_Window* window = nullptr;
		SDL_Renderer* renderer = nullptr;

		if (!SDL_CreateWindowAndRenderer("Dod Project", Constants::WIDTH,
			Constants::HEIGHT, 0, &window, &renderer))
			SDL_Log(SDL_GetError());

		SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
		SDL_RenderClear(renderer);
		SDL_RenderPresent(renderer);

		SDL_Texture* circleTexture = SDL_CreateTexture(
			renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 64, 64);

		SDL_SetTextureBlendMode(circleTexture, SDL_BLENDMODE_BLEND);

		SDL_SetRenderTarget(renderer, circleTexture);
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
		SDL_RenderClear(renderer);
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

		for (float w = 0; w <= 64; w++) {
			for (float h = 0; h <= 64; h++) {
				float dx = 32 - w;
				float dy = 32 - h;
				if ((dx * dx + dy * dy) <= 32 * 32) {
					SDL_RenderPoint(renderer, w, h);
				}
			}
		}

		SDL_SetRenderTarget(renderer, nullptr);
		SDL_SetTextureScaleMode(circleTexture, SDL_SCALEMODE_LINEAR);

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO io = ImGui::GetIO();
		(void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

		ImGui::StyleColorsDark();

		ImGuiStyle& style = ImGui::GetStyle();
		style.ScaleAllSizes(1);
		style.FontScaleDpi = 1;

		ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
		ImGui_ImplSDLRenderer3_Init(renderer);

		Uint64 Current = SDL_GetPerformanceCounter();
		Uint64 Last = 0;

		bool showDemoWindow = 1;
		bool running = true;

		float deltaTime = 0.f;

		SDL_Event currentEvent;

		while (running) {
			int col_nr = 0;

			Last = Current;
			Current = SDL_GetPerformanceCounter();

			deltaTime =
				(float)((float)(Current - Last) / SDL_GetPerformanceFrequency());

			while (SDL_PollEvent(&currentEvent)) {
				ImGui_ImplSDL3_ProcessEvent(&currentEvent);

				if (currentEvent.type == SDL_EVENT_QUIT) {
					SDL_Log("Closed the window");
					running = false;
				}
			}

			ImGui_ImplSDLRenderer3_NewFrame();
			ImGui_ImplSDL3_NewFrame();
			ImGui::NewFrame();

			{
				ImGui::Begin("Debug window");

				ImGui::SliderInt("Radius", &radius, 1, 10);
				ImGui::SliderInt("Asteroids", &spawnedAsteroids, 1, 100'000 - index);
				ImGui::SliderInt("Erase Count", &erasedAsteroids, 1, index);
				ImGui::ColorEdit3("Asteroids Color", color);

				if (ImGui::Button("Spawn")) {
					for (int i = 0; i < spawnedAsteroids; ++i) {
						centers[index] = vec2f{ (float)SDL_rand(Constants::WIDTH),
											  (float)SDL_rand(Constants::HEIGHT) };
						radiuses[index] = radius;
						speeds[index] = 50.f;
						velocities[index] =
							vec2f{ (float)SDL_cos(360 * SDL_randf()) * speeds[index],
								 (float)SDL_sin(360 * SDL_randf()) * speeds[index] };
						InsertAsteroid(grid, index, centers[index], radiuses[index]);
						index++;
					}
				}

				if (ImGui::Button("Erase"))
					if (index) {
						int eraseCount = std::min(erasedAsteroids, index);
						index -= eraseCount;
					}

				ImGui::Text("Application average %.1f FPS", 1 / deltaTime);
				ImGui::End();
			}

			UpdateGrid(grid, centers, index, radiuses);

			#pragma omp parallel for
			for (int i = 0; i < Constants::CELL_COUNT; i++) {
				AddCollision(collisions, col_nr, grid, Constants::CELL_SIZE * i, centers, radiuses);
				//ResolveCollision(grid, Constants::CELL_SIZE * i, centers, velocities,
					//radiuses);
			}
			
			ResolveCollision(collisions, col_nr, centers, radiuses, velocities);

			SDL_RenderClear(renderer);
			ImGui::Render();

			#pragma omp parallel for
			for (int i = 0; i < index; ++i) {
				Move(centers[i], velocities[i], deltaTime);
				BorderCollision(centers[i], velocities[i], radiuses[i]);
				//Draw(renderer, centers[i], radiuses[i]);
			}

			#pragma omp parallel for
			for (int i = 0; i < index; i++) {
				float radius = radiuses[i];
				float cx = centers[i].x;
				float cy = centers[i].y;

				int vertex_index = i * 4;
				int indices_index = i * 6;

				vertices[vertex_index + 0] = { {cx - radius, cy - radius}, {color[0], color[1], color[2], 1}, {0,0} };
				vertices[vertex_index + 1] = { {cx + radius, cy - radius}, {color[0], color[1], color[2], 1}, {1,0} };
				vertices[vertex_index + 2] = { {cx + radius, cy + radius}, {color[0], color[1], color[2], 1}, {1,1} };
				vertices[vertex_index + 3] = { {cx - radius, cy + radius}, {color[0], color[1], color[2], 1}, {0,1} };
			}


			SDL_RenderGeometry(renderer, circleTexture, vertices, index * 4, indices, index * 6);

			//for (int i = 0; i < index; ++i)

			SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
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
	}

	default:
	{
		std::cout << "Not a valid option\n";
		break;
	}
	}
	return 0;
}