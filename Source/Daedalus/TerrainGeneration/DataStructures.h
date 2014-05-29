#pragma once

#include "Engine.h"

namespace utils {
	struct Triangle {
		FVector points[3];

		Triangle(const FVector & p1, const FVector & p2, const FVector & p3) {
			points[0] = p1;
			points[1] = p2;
			points[2] = p3;
		}
	};

	struct GridCell {
		bool values[8];
		FVector points[8];

		void Initialize(
			const bool blf, const bool tlf,
			const bool blb, const bool tlb,
			const bool brf, const bool trf,
			const bool brb, const bool trb
			) {
			values[0] = blf; values[1] = brf; values[2] = brb; values[3] = blb;
			values[4] = tlf; values[5] = trf; values[6] = trb; values[7] = tlb;
			points[0] = FVector(0, 0, 0);
			points[1] = FVector(1, 0, 0);
			points[2] = FVector(1, 1, 0);
			points[3] = FVector(0, 1, 0);
			points[4] = FVector(0, 0, 1);
			points[5] = FVector(1, 0, 1);
			points[6] = FVector(1, 1, 1);
			points[7] = FVector(0, 1, 1);
		}
	};
}
