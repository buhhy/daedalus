#pragma once

#include "DataStructures.h"

namespace utils {
	template <typename T>
	Int64 HashFromVector(
		const Int64 seed,
		const Vector3D<T> & absolutePos
	) {
		Int64 collapsed = (
			absolutePos.X * 6151 +
			absolutePos.Y * 3079 +
			absolutePos.Z * 1543 + 769) ^ ((seed + 123456789012345) * 389);

		// FNV-1a hashing
		Int64 hash = 14695981039346656037ull;
		for (auto i = 0; i < 8; i++) {
			hash *= 1099511628211;
			hash ^= collapsed & 0xff;
			collapsed >>= 8;
		}

		return hash;
	}

	template <typename T>
	Int64 HashFromVector(
		const Int64 seed,
		const Vector2D<T> & absolutePos
	) {
		Int64 collapsed = (
			absolutePos.X * 6151 +
			absolutePos.Y * 3079 +  1543) ^ ((seed + 123456789012345) * 769);

		// FNV-1a hashing
		Int64 hash = 14695981039346656037ull;
		for (auto i = 0; i < 8; i++) {
			hash *= 1099511628211;
			hash ^= collapsed & 0xff;
			collapsed >>= 8;
		}

		return hash;
	}
}
