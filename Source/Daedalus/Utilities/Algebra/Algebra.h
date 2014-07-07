#pragma once

#include <Utilities/Integers.h>
#include <Utilities/Constants.h>

#include <functional>

namespace utils {
}

namespace std {
	template <typename T>
	inline void hashCombine(Int64 & seed, const T & v, const std::hash<T> & hasher) {
		seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	}
}
