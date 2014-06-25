#include <Daedalus.h>
#include "GraphDatastructures.h"

#include <unordered_map>

namespace utils {
	namespace voronoi {
	}

	using namespace halfedge;

	VoronoiGraph::VoronoiGraph(const DelaunayGraph & source) {
		// Generate all Voronoi vertices from circumcenters of faces
		auto dFaces = source.GetFaces();
		std::unordered_map<uint64_t, Vertex *> vVertices;
		for (auto i : dFaces)
			vVertices.insert({ i->FaceId(), new Vertex(i->GetCircumcircle().Center, i->FaceId()) });

	}
}
