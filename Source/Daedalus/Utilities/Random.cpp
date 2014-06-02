#include "Daedalus.h"
#include "Random.h"

namespace utils {
	double generateRandomNumber(
		const uint64 seed,
		const Vector3<int64> & absolutePos
	) {
		uint64 collapsed = (
			absolutePos.X * 6151 +
			absolutePos.Y * 3079 +
			absolutePos.Z * 1543 + 769) ^ seed * 389;

		// FNV-1a hashing
		uint64 hash = 14695981039346656037u;
		for (auto i = 0; i < 8; i++) {
			hash *= 1099511628211;
			hash ^= collapsed & 0xff;
			collapsed >>= 8;
		}

		return (double)hash / (double)MAX_uint64;
	}
}
