#include "BiomeRegionRenderer.h"
#include "SDLHelpers.h"

#include <Utilities/DataStructures.h>
#include <Utilities/Noise/Perlin.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <iostream>

const uint16_t SizeX = 800, SizeY = 800;

void DrawNoise(
	SDL_Renderer * renderer, const utils::PerlinNoise2D * generator,
	const uint16_t sizeX, const uint16_t sizeY, const float scale
) {
	double min = 2, max = -2;
	for (uint16_t x = 1; x <= sizeX; x++) {
		for (uint16_t y = 1; y <= sizeY; y++) {
			//double noise = (generator->Generate(x*0.12, y*0.12) * 1.3 + 1) * 0.5;
			double noise = generator->GenerateFractal(x * 0.005, y * 0.005, 6u, 0.5) * 0.5;
			if (noise < min) min = noise;
			if (noise > max) max = noise;
			uint8_t r = noise * 255, g = noise * 255, b = noise * 255;
			SDL_SetRenderDrawColor(renderer, r, g, b, SDL_ALPHA_OPAQUE);
			SDL_RenderDrawPoint(renderer, x, y);
		}
	}
	std::cout << min << " " << max << std::endl;
}

int main(int argc, char ** argv) {
	if (SDL_Init(SDL_INIT_EVERYTHING))
		Quit("SDL_Init Error: ");
	
	// Initialize SDL2_TTF library
	if (TTF_Init())
		Quit("TTF_Init Error: ", true);

	SDL_Window * window = SDL_CreateWindow("Daedalus Experimental", 100, 100, SizeX, SizeY, SDL_WINDOW_SHOWN);
	if (window == NULL)
		Quit("SDL_CreateWindow Error: ", true, true);

	SDL_Renderer * renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == NULL)
		Quit("SDL_CreateRenderer Error: ", true, true, window);
	
	TTF_Font * fontRegular = TTF_OpenFont("lato.ttf", 48);
	if (fontRegular == NULL)
		Quit("TTF_OpenFont() Error: ", true, true, window);

	BiomeRegionRenderer regionRenderer(SizeX, SizeY, fontRegular);

	bool running = true;
	bool updated = false;
	SDL_Event sdlEvent;
	auto generator = new utils::PerlinNoise2D(0);

	while (running) {
		while (SDL_PollEvent(&sdlEvent)) {
			if (sdlEvent.type == SDL_QUIT)
				running = false;
		}

		if (!updated) {
			SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
			SDL_RenderClear(renderer);
			regionRenderer.DrawBiomeRegion(renderer, { -28, 3});
			//DrawNoise(renderer, generator, SizeX, SizeY, 0.005f);
			SDL_RenderPresent(renderer);
			updated = true;
		}
	}

	delete generator;

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
