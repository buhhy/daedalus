#pragma once

namespace utils {
	template <typename T = double>
	struct Vector2 {
		T X;
		T Y;

		Vector2() {}
		Vector2(T x, T y): X(y), Y(y) {}

		inline void Reset(T x, T y) { X = x; Y = y; }

		/**
		 * Dot product.
		 */
		template <typename T1>
		inline double Dot(const Vector2<T1> other) const {
			return X * other.X + Y * other.Y;
		}

		/** Length squared. */
		inline double Length2() const { return X * X + Y * Y; }
		inline double Length() const { return FMath::Sqrt(Length2()); }

		inline FVector ToFVector(float z = 0.0) const { return FVector(X, Y, z); }

		template <typename T1>
		inline Vector2<T1> Cast() const { return Vector2<T1>(this->X, this->Y); }

		Vector2<double> Normalize() const;
	};
}

namespace std {
	template <typename T>
	struct hash<utils::Vector2<T> > {
		size_t operator()(const utils::Vector2<T> & v) const {
			int64 seed = 0;
			std::hashCombine(seed, v.X);
			std::hashCombine(seed, v.Y);
			return seed;
		}
	};
}

#include "Algebra2.h"
