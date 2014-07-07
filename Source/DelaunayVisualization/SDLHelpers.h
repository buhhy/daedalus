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
	const bool ttf = false, SDL_Window * window = NULL);

void ClearCanvas(SDL_Renderer * renderer);

void RenderText(
	SDL_Renderer * renderer, const char * text,
	const Int32 x0, const Int32 y0,
	TTF_Font * font, const Colour & colour);

void RenderCircle(
	SDL_Renderer * renderer, const Int32 x0, const Int32 y0,
	const Uint16 radius, const Colour & colour);

void RenderFilledCircle(
	SDL_Renderer * renderer, const Int32 x0, const Int32 y0,
	const Uint16 radius, const Colour & colour);

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

inline void RenderText(
	SDL_Renderer * renderer, const char * text,
	const Vector2<> & position,
	TTF_Font * font, const Colour & colour
) {
	RenderText(renderer, text, position.X, position.Y, font, colour);
}
