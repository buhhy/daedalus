#include <Daedalus.h>
#include "TerrainDataStructures.h"

namespace terrain {
	bool operator == (const ChunkPositionVector & lhs, const ChunkPositionVector & rhs) {
		return lhs.ChunkOffset == rhs.ChunkOffset && lhs.InnerOffset == rhs.InnerOffset;
	}

	bool operator != (const ChunkPositionVector & lhs, const ChunkPositionVector & rhs) {
		return !(lhs == rhs);
	}
}
