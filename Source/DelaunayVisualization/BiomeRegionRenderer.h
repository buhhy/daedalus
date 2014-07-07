#pragma once

#include "SDLHelpers.h"
#include <Models/Terrain/BiomeRegionLoader.h>
#include <Utilities/Algebra/Algebra2D.h>

#include <SDL2/SDL.h>
#include <SDL2_ttf/SDL_ttf.h>

#include <unordered_set>
#include <sstream>

using namespace utils;
using namespace terrain;

class DelaunayRenderer {
public:
	static const Colour VertexColour;
	static const Colour EdgeColour;
	static const Colour AddedEdgeColour;
	static const Colour HullEdgeColour;
	static const Colour HullIndexColour;
	static const Colour TangentStartColour;
	static const Colour TangentEndColour;

	using EdgeSet = std::unordered_set<delaunay::Edge>;

	Uint16 Width;
	Uint16 Height;
	
	FontPack & Fonts;

	SDL_Renderer * Renderer;

	Vector2D<> GetPointPosition(const Vector2D<> p) const {
		return { p.X * Width, (1 - p.Y) * Height };
	}

	Vector2D<> GetVertexPosition(const delaunay::Vertex * vert) const {
		return GetPointPosition(vert->GetPoint());
	}

	DelaunayRenderer(
		SDL_Renderer * renderer,
		const Uint16 width, const Uint16 height,
		FontPack & fonts
	) : Width(width), Height(height), Renderer(renderer), Fonts(fonts)
	{}

	/**
	 * Drawing helpers
	 */

	void AddHullEdges(
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

	void Clear() { ClearCanvas(Renderer); }
	void Present() { SDL_RenderPresent(Renderer); }

	void DrawPoint(
		const Vector2D<> point, const Uint16 radius,
		const Colour & colour, const bool filled = false
	) {
		if (filled)
			RenderFilledCircle(Renderer, GetPointPosition(point), radius, colour);
		else
			RenderCircle(Renderer, GetPointPosition(point), radius, colour);
	}

	void DrawVertex(
		const delaunay::Vertex * vertex,
		const Uint16 radius,
		const Colour & colour = VertexColour
	) {
		DrawPoint(vertex->GetPoint(), radius, colour);
	}

	void DrawVertices(
		const std::vector<const delaunay::Vertex *> & vertices,
		const Colour & colour = VertexColour
	) {
		// Draw vertices
		for (auto & vertex : vertices)
			DrawVertex(vertex, 1, colour);
	}

	void DrawEdges(const EdgeSet & edges, const Colour & colour = EdgeColour) {
		SDL_SetRenderDrawColor(Renderer, colour.X, colour.Y, colour.Z, SDL_ALPHA_OPAQUE);
		for (auto & edge : edges)
			RenderLine(Renderer, GetVertexPosition(edge.Start), GetVertexPosition(edge.End));
	}

	void DrawAddedFaces(
		const DelaunayBuilderDAC2D::AddedFaceList & added,
		const Colour & colour = EdgeColour
	) {
		EdgeSet edges;
		for (auto & entry : added) {
			for (Uint8 i = 0; i < entry.size(); i++)
				edges.insert(delaunay::Edge(entry[i], entry[(i + 1) % entry.size()]));
		}
		DrawEdges(edges, colour);
	}

	void DrawHullVertexNumbers(
		const delaunay::ConvexHull & hull,
		const Colour & colour = HullIndexColour
	) {
		Vector2D<> offset(6, 4);
		std::stringstream stream;

		for (Uint64 i = 0; i < hull.Size(); i++) {
			stream.str("");
			stream << i;
			RenderText(
				Renderer, stream.str().c_str(),
				GetVertexPosition(hull[i]) + offset,
				Fonts.S10, colour);
		}
	}
};

class DelaunayDAC2DDebugger : public IDelaunayDAC2DDebug {
private:
	DelaunayRenderer & Renderer;
	bool NextStep;
	bool Finish;

	using EdgeSet = DelaunayRenderer::EdgeSet;

public:
	DelaunayDAC2DDebugger(DelaunayRenderer & renderer) :
		Renderer(renderer), NextStep(false), Finish(false)
	{}

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
		if (!NextStep && !Finish) {
			Renderer.Clear();
		
			EdgeSet leftEdges, rightEdges, hullEdges;

			// Different graphs
			if (leftGraph.GraphOffset() != rightGraph.GraphOffset()) {
				// Different graphs
				leftEdges = leftGraph.GetUniqueEdges();
				rightEdges = rightGraph.GetUniqueEdges();

				Renderer.DrawEdges(leftEdges);
				Renderer.DrawEdges(rightEdges);

				Renderer.DrawVertices(leftGraph.GetVertices());
				Renderer.DrawVertices(rightGraph.GetVertices());
			} else {
				// Same graphs, no need to draw both graphs
				leftEdges = leftGraph.GetUniqueEdges();
				rightEdges = leftEdges;
				
				Renderer.DrawEdges(leftGraph.GetUniqueEdges());
				Renderer.DrawVertices(leftGraph.GetVertices());
			}

			Renderer.AddHullEdges(hullEdges, leftHull, leftEdges);
			Renderer.AddHullEdges(hullEdges, rightHull, rightEdges);

			Renderer.DrawEdges(hullEdges, DelaunayRenderer::HullEdgeColour);

			Renderer.DrawAddedFaces(leftAddedFaces, DelaunayRenderer::AddedEdgeColour);
			Renderer.DrawAddedFaces(rightAddedFaces, DelaunayRenderer::AddedEdgeColour);

			// Draw start and end points
			// Lower tangent is orange
			Renderer.DrawVertex(leftHull[lowerTangent.LeftId], 4, DelaunayRenderer::TangentStartColour);
			Renderer.DrawVertex(rightHull[lowerTangent.RightId], 4, DelaunayRenderer::TangentStartColour);
			// Upper tangent is red
			Renderer.DrawVertex(leftHull[upperTangent.LeftId], 4, DelaunayRenderer::TangentEndColour);
			Renderer.DrawVertex(rightHull[upperTangent.RightId], 4, DelaunayRenderer::TangentEndColour);

			// Draw centroid
			//RenderFilledCircle(Renderer, leftHull.Centroid() * Vector2D<>(Width, Height), 4, { 0, 131, 129 });
			//RenderFilledCircle(Renderer, rightHull.Centroid() * Vector2D<>(Width, Height), 4, { 0, 131, 129 });

			// Draw convex hull points
			Renderer.DrawHullVertexNumbers(leftHull);
			Renderer.DrawHullVertexNumbers(rightHull);
		
			Renderer.Present();

			// Wait for the enter key, right ctrl skips to next merge step
			SDL_Event sdlEvent;
			bool done = false;
			while (!done) {
				while (SDL_PollEvent(&sdlEvent)) {
					if (sdlEvent.type == SDL_QUIT)
						exit(0);

					if (sdlEvent.type == SDL_KEYUP) {
						if (sdlEvent.key.keysym.scancode == SDL_SCANCODE_F11)
							done = true;
						if (sdlEvent.key.keysym.scancode == SDL_SCANCODE_F5) {
							Finish = true;
							done = true;
						}
						if (sdlEvent.key.keysym.scancode == SDL_SCANCODE_F10) {
							NextStep = true;
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
		NextStep = false;
	}
};

class BiomeRegionRenderer {
private:
	Int64 Seed;
	
	std::shared_ptr<DelaunayDAC2DDebugger> Debugger;
	std::shared_ptr<events::EventBus> MockBus;
	BiomeGeneratorParameters GenParams;
	BiomeRegionLoader RegionLoader;
	DelaunayRenderer Renderer;

public:
	BiomeRegionRenderer(
		SDL_Renderer * renderer,
		const Uint16 width, const Uint16 height,
		FontPack & fonts
	) : Seed(12345678), Renderer(renderer, width, height, fonts),
		Debugger(new DelaunayDAC2DDebugger(Renderer)),
		MockBus(new events::EventBus()),
		GenParams({
			16,              // Number of grid cells along a single axis
			Seed,            // Seed
			4,               // Number of buffer cells in grid
			1,               // Minimum bound of number of points
			1,               // Maximum bound of number of points
			16 * 0x10        // Size of the biome region in real units along a single axis
		}),
		RegionLoader(
			GenParams, MockBus,
			BiomeRegionLoader::DelaunayBuilderPtr(new DelaunayBuilderDAC2D(0, Debugger)),
			1)
	{}

	void DrawBiomeRegion(
		const BiomeRegionOffsetVector & offset
	) {
		auto region = RegionLoader.GetBiomeRegionAt({ 0, 0 });
		const auto & graph = region->DelaunayGraph;
		const auto & edges = graph.GetUniqueEdges();

		Renderer.Clear();

		RenderText(Renderer.Renderer, "Done!",
			Renderer.GetPointPosition({ 1.0, 0.5 }), Renderer.Fonts.S48, { 244, 60, 48 });
		
		// Draw edges
		Renderer.DrawEdges(edges, { 36, 120, 195 });

		// Nearest biome test
		//for (Uint32 y = 40; y < 50; y++) {
		//	for (Uint32 x = 0; x < Renderer.Width; x++) {
		//		double dx = GenParams.BiomeScale * x / Renderer.Width;
		//		double dy = GenParams.BiomeScale * y / Renderer.Height;
		//		auto id = RegionLoader.FindNearestBiomeId({ dx, dy });
		//		auto data = RegionLoader.GetBiomeAt(id);
		//		Uint8 colour = ((Uint8) data->GetElevation() / 2.0) * 255;
		//		//SDL_SetRenderDrawColor(Renderer.Renderer, colour, colour, colour, SDL_ALPHA_OPAQUE);
		//		//SDL_RenderDrawPoint(Renderer.Renderer, x, y);
		//	}
		//}
		//auto testp = Vector2D<>(25, 25);
		//auto relp = GenParams.GetInnerRegionPosition(testp, { 0, 0 });
		//Renderer.DrawPoint(relp, 4, { 35, 35, 35 }, true);
		//auto id = RegionLoader.FindNearestBiomeId(testp);
		//auto data = RegionLoader.GetBiomeAt(id);
		//Renderer.DrawPoint(data->GetLocalPosition(), 8, { 200, 0, 0 });

		Renderer.Present();
	}
};
