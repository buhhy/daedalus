#include <Daedalus.h>
#include "Algebra.h"

namespace utils {
	bool ECompare(
		const double v1, const double v2,
		const double maxDiff, const double maxRelDiff
	) {
		// Differences close to 0 cause problems with relative error.
		const double diff = std::abs(v2 - v1);
		if (diff <= maxDiff)
			return true;
		const double max = std::max(v1, v2);
		return diff <= max * maxRelDiff;
	}
}
