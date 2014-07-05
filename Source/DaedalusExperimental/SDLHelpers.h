#pragma once

#include <SDL2/SDL.h>
#include <SDL2_ttf/SDL_ttf.h>

#include <Utilities/DataStructures.h>

#include <iostream>

using utils::Vector2;
using utils::Colour;

struct FontPack {
	TTF_Font * S10;
	TTF_Font * S16;
	TTF_Font * S24;
	TTF_Font * S36;
	TTF_Font * S48;

	FontPack(const char * name) :
		S10(TTF_OpenFont(name, 10)),
		S16(TTF_OpenFont(name, 16)),
		S24(TTF_OpenFont(name, 24)),
		S36(TTF_OpenFont(name, 36)),
		S48(TTF_OpenFont(name, 48))
	{}

	bool Loaded() const {
		return S10 && S16 && S24 && S36 && S48;
	}
};

void Quit(
	const char * error, const bool sdl = false,
	const bool ttf = false, SDL_Window * window = NULL
) {
	const char * errorMsg = SDL_GetError();
	std::cerr << error << errorMsg << std::endl;
	if (window) SDL_DestroyWindow(window);
	if (ttf) TTF_Quit();
	if (sdl) SDL_Quit();
	exit(1);
}

void ClearCanvas(SDL_Renderer * renderer) {
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(renderer);
}

void RenderText(
	SDL_Renderer * renderer, const char * text,
	const Int32 x0, const Int32 y0,
	TTF_Font * font, const Colour & colour
) {
		SDL_SetRenderDrawColor(renderer, 133, 186, 233, SDL_ALPHA_OPAQUE);
		auto textObj = TTF_RenderText_Blended(font, text, { colour.X, colour.Y, colour.Z });
		SDL_Rect rect = textObj->clip_rect;
		rect.x = x0; rect.y = y0;
		SDL_RenderCopy(renderer, SDL_CreateTextureFromSurface(renderer, textObj), NULL, &rect);
}

void RenderCircle(
	SDL_Renderer * renderer, const Int32 x0, const Int32 y0,
	const Uint16 radius, const Colour & colour
) {
	Int32 x = radius, y = 0;
	Int32 radiusError = 1-x;

	SDL_Point points[8];

	SDL_SetRenderDrawColor(renderer, colour.X, colour.Y, colour.Z, SDL_ALPHA_OPAQUE);
 
	while (x >= y) {
		points[0] = { x + x0, y + y0 };
		points[1] = { y + x0, x + y0 };
		points[2] = { -x + x0, y + y0 };
		points[3] = { -y + x0, x + y0 };
		points[4] = { -x + x0, -y + y0 };
		points[5] = { -y + x0, -x + y0 };
		points[6] = { x + x0, -y + y0 };
		points[7] = { y + x0, -x + y0 };
		y++;

		SDL_RenderDrawPoints(renderer, points, 8);

		if (radiusError<0) {
			radiusError += 2 * y + 1;
		} else {
			x--;
			radiusError+= 2 * (y - x + 1);
		}
	}
}

void RenderFilledCircle(
	SDL_Renderer * renderer, const Int32 x0, const Int32 y0,
	const Uint16 radius, const Colour & colour
) {
	for (Uint16 r = 0; r <= radius; r++)
		RenderCircle(renderer, x0, y0, r, colour);
}

// Convenience wrappers

inline void RenderLine(
	SDL_Renderer * renderer,
	const Vector2<> & start, const Vector2<> & end
) {
	SDL_RenderDrawLine(renderer, start.X, start.Y, end.X, end.Y);
}

inline void RenderCircle(
	SDL_Renderer * renderer, const Vector2<> & position,
	const Uint16 radius, const Colour & colour
) {
	RenderCircle(renderer, std::round(position.X), std::round(position.Y), radius, colour);
}

inline void RenderFilledCircle(
	SDL_Renderer * renderer, const Vector2<> & position,
	const Uint16 radius, const Colour & colour
) {
	RenderFilledCircle(renderer, std::round(position.X), std::round(position.Y), radius, colour);
}

void RenderText(
	SDL_Renderer * renderer, const char * text,
	const Vector2<> & position,
	TTF_Font * font, const Colour & colour
) {
	RenderText(renderer, text, position.X, position.Y, font, colour);
}
