#include <SDL2/SDL.h>

#include <Utilities/DataStructures.h>
#include <Utilities/Noise/Perlin.h>

#include <iostream>

const uint16_t SizeX = 800, SizeY = 600;

void DrawNoise(
	SDL_Renderer * renderer, const utils::PerlinNoise * generator,
	const uint16_t sizeX, const uint16_t sizeY, const float scale
) {
	for (uint16_t x = 0; x < sizeX; x++) {
		for (uint16_t y = 0; y < sizeY; y++) {
			auto noise = generator->Generate(scale * x, scale * y);
			uint8_t r = noise * 256, g = noise * 256, b = noise * 256;
			std::cout << r << g << b << " ";
			SDL_SetRenderDrawColor(renderer, r, g, b, SDL_ALPHA_OPAQUE);
			SDL_RenderDrawPoint(renderer, x, y);
		}
	}
	std::cout << std::endl;
}

int main(int argc, char ** argv) {
	if (SDL_Init(SDL_INIT_EVERYTHING)) {
		std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return 1;
	}

	SDL_Window * window = SDL_CreateWindow("Daedalus Experimental", 100, 100, SizeX, SizeY, SDL_WINDOW_SHOWN);
	if (window == NULL) {
		std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return 1;
	}

	SDL_Renderer * renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == NULL) {
		SDL_DestroyWindow(window);
		std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return 1;
	}

	bool running = true;
	SDL_Event sdlEvent;
	auto generator = new utils::PerlinNoise();

	while (running) {
		while (SDL_PollEvent(&sdlEvent)) {
			if (sdlEvent.type == SDL_QUIT)
				running = false;
		}
		
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
		SDL_RenderClear(renderer);
		DrawNoise(renderer, generator, SizeX, SizeY, 0.15);
		SDL_RenderPresent(renderer);
	}

	delete generator;

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
