#pragma once

#include <Utilities/Integers.h>
#include <Utilities/Constants.h>

#include <algorithm>
#include <functional>

namespace utils {
	template <typename T>
	inline Int8 Sign(const T value) { return (T(0) < value) - (value < T(0)); }

	/**
	 * Custom math functions for floating point operations that account for precision issues.
	 * Relevant article: http://www.cygnus-software.com/papers/comparingfloats/comparingfloats.htm
	 */
	inline Int64 ECeil(const double value, const double maxDiff = DOUBLE_EPSILON) {
		return std::ceil(value - maxDiff);
	}

	inline Int64 EFloor(const double value, const double maxDiff = DOUBLE_EPSILON) {
		return std::floor(value + maxDiff);
	}

	bool ECompare(
		const double v1, const double v2,
		const double maxDiff = DOUBLE_EPSILON,
		const double maxRelDiff = DOUBLE_EPSILON);
}

namespace std {
	template <typename T>
	inline void hashCombine(Int64 & seed, const T & v, const std::hash<T> & hasher) {
		seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	}

	struct CustomUint64Hash {
		size_t operator()(const Uint64 v) const { return v; }
	};

	struct CustomInt64Hash {
		size_t operator()(const Int64 v) const { return (unsigned) v; }
	};
}
