#pragma once

#include "DataStructures.h"

namespace utils {
	template <typename T>
	int64_t HashFromVector(
		const int64_t seed,
		const Vector3<T> & absolutePos
	) {
		int64_t collapsed = (
			absolutePos.X * 6151 +
			absolutePos.Y * 3079 +
			absolutePos.Z * 1543 + 769) ^ ((seed + 123456789012345) * 389);

		// FNV-1a hashing
		int64_t hash = 14695981039346656037;
		for (auto i = 0; i < 8; i++) {
			hash *= 1099511628211;
			hash ^= collapsed & 0xff;
			collapsed >>= 8;
		}

		return hash;
	}

	template <typename T>
	int64_t HashFromVector(
		const int64_t seed,
		const Vector2<T> & absolutePos
	) {
		int64_t collapsed = (
			absolutePos.X * 6151 +
			absolutePos.Y * 3079 +  1543) ^ ((seed + 123456789012345) * 769);

		// FNV-1a hashing
		int64_t hash = 14695981039346656037;
		for (auto i = 0; i < 8; i++) {
			hash *= 1099511628211;
			hash ^= collapsed & 0xff;
			collapsed >>= 8;
		}

		return hash;
	}
}
