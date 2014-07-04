#pragma once

#include "SDLHelpers.h"
#include <Models/Terrain/BiomeRegionLoader.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

class BiomeRegionRenderer {
private:
	uint64_t Seed;
	uint16_t Width;
	uint16_t Height;
	std::shared_ptr<events::EventBus> MockBus;
	terrain::BiomeRegionLoader RegionLoader;

	TTF_Font * FontRegular;

public:
	BiomeRegionRenderer(const uint16_t width, const uint16_t height, TTF_Font * fontRegular) :
		Seed(12345678), Width(width), Height(height),
		MockBus(new events::EventBus()),
		RegionLoader(
			{
				16,              // Number of grid cells along a single axis
				Seed,            // Seed
				4,               // Number of buffer cells in grid
				1,               // Minimum bound of number of points
				1,               // Maximum bound of number of points
				16 * 0x10        // Size of the biome region in real units along a single axis
			},
			MockBus,
			terrain::BiomeRegionLoader::DelaunayBuilderPtr(new utils::DelaunayDivideAndConquerBuilder2D(2))),
		FontRegular(fontRegular)
	{}

	void DrawBiomeRegion(
		SDL_Renderer * renderer,
		const terrain::BiomeRegionOffsetVector & offset
	) {
		auto region = RegionLoader.GetBiomeRegionAt(offset);
		const auto & graph = region->DelaunayGraph;
		const auto & vertices = graph.GetVertices();
		const auto & edges = graph.GetUniqueEdges();
		const auto & faces = graph.GetFaces();
		
		// Draw vertices
		SDL_SetRenderDrawColor(renderer, 2, 84, 158, SDL_ALPHA_OPAQUE);
		for (auto & vertex : vertices)
			SDL_RenderDrawPoint(renderer, std::round(vertex->GetPoint().X * Width), std::round(vertex->GetPoint().Y * Height));
		
		// Draw edges
		SDL_SetRenderDrawColor(renderer, 36, 120, 195, SDL_ALPHA_OPAQUE);
		for (auto & edge : edges)
			SDL_RenderDrawLine(renderer,
				edge.Start->GetPoint().X * Width, edge.Start->GetPoint().Y * Height,
				edge.End->GetPoint().X * Width, edge.End->GetPoint().Y * Height);
		
		// Draw faces
		SDL_SetRenderDrawColor(renderer, 133, 186, 233, SDL_ALPHA_OPAQUE);
		auto text = TTF_RenderText_Blended(FontRegular, "This is a test text, whoa!", {0, 0, 0});
		SDL_Rect rect = text->clip_rect;
		rect.x = 100; rect.y = 100;
		
		SDL_RenderCopy(renderer, SDL_CreateTextureFromSurface(renderer, text), NULL, &rect);
		
			/*SDL_(renderer,
				edge.Start->GetPoint().X * Width, edge.Start->GetPoint().Y * Height,
				edge.End->GetPoint().X * Width, edge.End->GetPoint().Y * Height);*/
	}

};
