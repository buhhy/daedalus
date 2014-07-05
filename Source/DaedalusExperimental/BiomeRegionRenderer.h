#pragma once

#include "SDLHelpers.h"
#include <Models/Terrain/BiomeRegionLoader.h>
#include <Utilities/Algebra/Algebra2.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <unordered_set>

using namespace utils;
using namespace terrain;

class DelaunayDAC2DDebugger : public IDelaunayDAC2DDebug {
private:
	using EdgeSet = std::unordered_set<delaunay::Edge>;

	bool Skipping;

	Uint16 Width;
	Uint16 Height;

	SDL_Renderer * Renderer;
	TTF_Font * FontRegular;

	void AddEdges(
		EdgeSet & edges,
		const delaunay::ConvexHull & hull,
		const EdgeSet & compareEdges
	) const {
		auto size = hull.Size();
		for (Uint64 i = 0, j = 1; i < size; i++, j++) {
			if (j >= size) j -= size;
			auto edge = delaunay::Edge(hull[i], hull[j]);
			// The convex hull vertex exists in the list of non-deleted edges
			if (compareEdges.find(edge) != compareEdges.end())
				edges.insert(edge);
		}
	}

public:
	DelaunayDAC2DDebugger(
		SDL_Renderer * renderer,
		const Uint16 width, const Uint16 height,
		TTF_Font * fontRegular
	) : Width(width), Height(height), Skipping(false),
		Renderer(renderer), FontRegular(fontRegular)
	{}

	/**
	 * Drawing helpers
	 */

	void DrawVertex(const delaunay::Vertex * vertex, const Uint16 radius, const Colour & colour) {
		RenderCircle(
			Renderer,
			std::round(vertex->GetPoint().X * Width),
			std::round(vertex->GetPoint().Y * Height),
			radius, colour);
	}

	void DrawVertices(const std::vector<const delaunay::Vertex *> & vertices, const Colour & colour) {
		// Draw vertices
		for (auto & vertex : vertices)
			DrawVertex(vertex, 1, colour);
	}

	void DrawEdges(const EdgeSet & edges, const Colour & colour) {
		SDL_SetRenderDrawColor(Renderer, colour.X, colour.Y, colour.Z, SDL_ALPHA_OPAQUE);
		for (auto & edge : edges) {
			SDL_RenderDrawLine(Renderer,
				edge.Start->GetPoint().X * Width, edge.Start->GetPoint().Y * Height,
				edge.End->GetPoint().X * Width, edge.End->GetPoint().Y * Height);
		}
	}

	void DrawAddedFaces(
		const DelaunayBuilderDAC2D::AddedFaceList & added,
		const Colour & colour
	) {
		EdgeSet edges;
		for (auto & entry : added) {
			for (Uint8 i = 0; i < entry.size(); i++)
				edges.insert(delaunay::Edge(entry[i], entry[(i + 1) % entry.size()]));
		}
		DrawEdges(edges, colour);
	}

	/**
	 * Algorithm event hooks
	 */

	virtual void MergeStep(
		const DelaunayGraph & leftGraph,
		const DelaunayGraph & rightGraph,
		const delaunay::ConvexHull & leftHull,
		const delaunay::ConvexHull & rightHull,
		const delaunay::Tangent & upperTangent,
		const delaunay::Tangent & lowerTangent,
		const DelaunayBuilderDAC2D::AddedFaceList & leftAddedFaces,
		const DelaunayBuilderDAC2D::AddedFaceList & rightAddedFaces
	) override {
		if (!Skipping) {
			ClearCanvas(Renderer);
		
			EdgeSet leftEdges, rightEdges, hullEdges;

			// Different graphs
			if (leftGraph.GraphOffset() != rightGraph.GraphOffset()) {
				// Different graphs
				leftEdges = leftGraph.GetUniqueEdges();
				rightEdges = rightGraph.GetUniqueEdges();

				DrawEdges(leftEdges, { 36, 120, 195 });
				DrawEdges(rightEdges, { 36, 120, 195 });

				DrawVertices(leftGraph.GetVertices(), { 9, 54, 95 });
				DrawVertices(rightGraph.GetVertices(), { 9, 54, 95 });
			} else {
				// Same graphs, no need to draw both graphs
				leftEdges = leftGraph.GetUniqueEdges();
				rightEdges = leftEdges;
				
				DrawEdges(leftGraph.GetUniqueEdges(), { 36, 120, 195 });
				DrawVertices(leftGraph.GetVertices(), { 9, 54, 95 });
			}

			AddEdges(hullEdges, leftHull, leftEdges);
			AddEdges(hullEdges, rightHull, rightEdges);

			DrawEdges(hullEdges, { 232, 183, 49 });

			DrawAddedFaces(leftAddedFaces, { 24, 165, 37 });
			DrawAddedFaces(rightAddedFaces, { 24, 165, 37 });

			// Draw start and end points
			// Lower tangent is orange
			DrawVertex(leftHull[lowerTangent.LeftId], 4, { 245, 144, 46 });
			DrawVertex(rightHull[lowerTangent.RightId], 4, { 245, 144, 46 });
			// Upper tangent is red
			DrawVertex(leftHull[upperTangent.LeftId], 4, { 212, 25, 0 });
			DrawVertex(rightHull[upperTangent.RightId], 4, { 212, 25, 0 });

			// Draw centroid
			RenderFilledCircle(Renderer, leftHull.Centroid() * Vector2<>(Width, Height), 4, { 0, 131, 129 });
			RenderFilledCircle(Renderer, rightHull.Centroid() * Vector2<>(Width, Height), 4, { 0, 131, 129 });
		
			SDL_RenderPresent(Renderer);

			// Wait for the enter key, right ctrl skips to next merge step
			SDL_Event sdlEvent;
			bool done = false;
			while (!done) {
				while (SDL_PollEvent(&sdlEvent)) {
					if (sdlEvent.type == SDL_QUIT)
						exit(0);

					if (sdlEvent.type == SDL_KEYUP) {
						if (sdlEvent.key.keysym.scancode == SDL_SCANCODE_RETURN)
							done = true;
						if (sdlEvent.key.keysym.scancode == SDL_SCANCODE_RCTRL) {
							Skipping = true;
							done = true;
						}
					}
				}
			}
		}
	}
	
	virtual void StartMergeStep(
		const DelaunayGraph & graph, const Uint32 subdivisionDepth
	) override {
		Skipping = false;
	}
};

class BiomeRegionRenderer {
private:
	Uint64 Seed;
	Uint16 Width;
	Uint16 Height;
	std::shared_ptr<DelaunayDAC2DDebugger> Debugger;
	std::shared_ptr<events::EventBus> MockBus;
	BiomeRegionLoader RegionLoader;

	SDL_Renderer * Renderer;
	TTF_Font * FontRegular;

public:
	BiomeRegionRenderer(
		SDL_Renderer * renderer,
		const Uint16 width, const Uint16 height,
		TTF_Font * fontRegular
	) : Seed(12345678), Width(width), Height(height),
		Renderer(renderer),
		Debugger(new DelaunayDAC2DDebugger(renderer, width, height, fontRegular)),
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
			BiomeRegionLoader::DelaunayBuilderPtr(new DelaunayBuilderDAC2D(0, Debugger)),
			0),
		FontRegular(fontRegular)
	{}

	void DrawBiomeRegion(
		const BiomeRegionOffsetVector & offset
	) {
		auto region = RegionLoader.GetBiomeRegionAt(offset);
		const auto & graph = region->DelaunayGraph;
		const auto & edges = graph.GetUniqueEdges();
		ClearCanvas(Renderer);

		RenderText(Renderer, "Test text, whoa!", 100, 400, FontRegular, { 244, 60, 48 });
		
		// Draw edges
		Debugger->DrawEdges(edges, { 36, 120, 195 });
		
		SDL_RenderPresent(Renderer);
	}
};
