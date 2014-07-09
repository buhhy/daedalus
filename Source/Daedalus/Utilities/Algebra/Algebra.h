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

	struct CustomUint64Hash {
		size_t operator()(const Uint64 v) const { return v; }
	};

	struct CustomInt64Hash {
		size_t operator()(const Int64 v) const { return (unsigned) v; }
	};
}
