#include <Daedalus.h>
#include "Algebra.h"

namespace utils {
	bool EEq(
		const double v1, const double v2,
		const double maxDiff, const double maxRelDiff
	) {
		// Differences close to 0 cause problems with relative error.
		const double diff = std::abs(v1 - v2);
		if (diff <= maxDiff)
			return true;
		const double max = std::max(v1, v2);
		return diff <= std::abs(max * maxRelDiff);
	}

	bool EGTE(
		const double v1, const double v2,
		const double maxDiff, const double maxRelDiff
	) {
		// Differences close to 0 cause problems with relative error.
		const double diff = v1 - v2;
		if (diff >= -maxDiff)
			return true;
		const double max = std::max(v1, v2);
		return diff >= -std::abs(max * maxRelDiff);
	}

	bool EGT(
		const double v1, const double v2,
		const double maxDiff, const double maxRelDiff
	) {
		// Differences close to 0 cause problems with relative error.
		const double diff = v1 - v2;
		if (diff > maxDiff)
			return true;
		const double max = std::max(v1, v2);
		return diff > std::abs(max * maxRelDiff);
	}

	bool ELTE(
		const double v1, const double v2,
		const double maxDiff, const double maxRelDiff
	) {
		// Differences close to 0 cause problems with relative error.
		const double diff = v1 - v2;
		if (diff <= maxDiff)
			return true;
		const double max = std::max(v1, v2);
		return diff <= std::abs(max * maxRelDiff);
	}

	bool ELT(
		const double v1, const double v2,
		const double maxDiff, const double maxRelDiff
	) {
		// Differences close to 0 cause problems with relative error.
		const double diff = v1 - v2;
		if (diff < -maxDiff)
			return true;
		const double max = std::max(v1, v2);
		return diff < -std::abs(max * maxRelDiff);
	}
}
