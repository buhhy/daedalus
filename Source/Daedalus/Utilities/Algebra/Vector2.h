#pragma once

#include <functional>

namespace utils {
	template <typename T = double>
	struct Vector2 {
		T X;
		T Y;

		Vector2() {}
		Vector2(const T & x, const T & y): X(x), Y(y) {}
		Vector2(const Vector2<T> & copy): Vector2(copy.X, copy.Y) {}

		inline void Reset(const T & x, const T & y) { X = x; Y = y; }
		inline void Reset(const Vector2<T> & other) { Reset(other.X, other.Y); }

		/** Dot product. */
		template <typename T1>
		inline double Dot(const Vector2<T1> other) const {
			return X * other.X + Y * other.Y;
		}

		/**
		 * Takes the determinant of the 2 vectors, effectively finding the winding order of
		 * the 2 vectors.
		 * | x1 y1 |
		 * | x2 y2 |
		 */
		inline T Determinant(const Vector2<T> other) const {
			return X * other.Y - Y * other.X;
		}

		/** Length squared. */
		inline double Length2() const { return (double) X * X + (double) Y * Y; }
		inline double Length() const { return std::abs(Length2()); }

		template <typename T1>
		inline Vector2<T1> Cast() const { return Vector2<T1>((T1) this->X, (T1) this->Y); }

		Vector2<double> Normalize() const;
	};
}

namespace std {
	template <typename T>
	struct hash<utils::Vector2<T> > {
		size_t operator()(const utils::Vector2<T> & v) const {
			int64_t seed = 0;
			std::hashCombine(seed, v.X);
			std::hashCombine(seed, v.Y);
			return (unsigned) seed;
		}
	};
}
