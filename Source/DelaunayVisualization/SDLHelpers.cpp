#include "SDLHelpers.h"

void Quit(
	const char * error, const bool sdl,
	const bool ttf, SDL_Window * window
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
