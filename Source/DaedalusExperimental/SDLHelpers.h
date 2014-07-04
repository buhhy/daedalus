#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <iostream>

void Quit(const char * error, const bool sdl = false, const bool ttf = false, SDL_Window * window = NULL) {
	const char * errorMsg = SDL_GetError();
	std::cerr << error << errorMsg << std::endl;
	if (window) SDL_DestroyWindow(window);
	if (ttf) TTF_Quit();
	if (sdl) SDL_Quit();
	exit(1);
}
