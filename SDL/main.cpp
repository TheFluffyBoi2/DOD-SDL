#include <SDL3/SDL.h>

int main(int argc, char* argv[]) {
	constexpr int width{ 800 };
	constexpr int height{ 600 };
	bool is_running{ true };


	if (!SDL_Init(SDL_INIT_VIDEO)) {
		SDL_Log(SDL_GetError());
		return 1;
	}

	SDL_Window* window{ nullptr };
	SDL_Renderer* renderer{ nullptr };

	if (!SDL_CreateWindowAndRenderer("Data oriented design", width, height, SDL_WINDOW_RESIZABLE, &window, &renderer)) {
		SDL_Log(SDL_GetError());
		return 1;
	}

	SDL_FRect rectangle{ width/4, height/4, width/2, height/2 };
	
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(renderer);

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderRect(renderer, &rectangle);
	SDL_RenderFillRect(renderer, &rectangle);

	SDL_RenderPresent(renderer);

	SDL_Event event;

	while (is_running) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_EVENT_QUIT) {
				is_running = false;
				SDL_Log("Closed the window");
			}
		}
	}

	SDL_Quit();

	return 0;
}