#pragma once

#include "DataStructures.h"

namespace utils {
	template <typename T>
	double hashFromVector(
		const int64 seed,
		const Vector3<T> & absolutePos
	) {
		int64 collapsed = (
			absolutePos.X * 6151 +
			absolutePos.Y * 3079 +
			absolutePos.Z * 1543 + 769) ^ seed * 389;

		// FNV-1a hashing
		int64 hash = 14695981039346656037;
		for (auto i = 0; i < 8; i++) {
			hash *= 1099511628211;
			hash ^= collapsed & 0xff;
			collapsed >>= 8;
		}

		return (double) hash / (double) MAX_uint64;
	}
}
