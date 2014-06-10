#pragma once

#include "Vector2.h"

namespace utils {
	/**
	* A standard XYZ vector that can be used as a hashmap key.
	*/
	template<typename T>
	struct Vector3 {
		T X;
		T Y;
		T Z;

		Vector3() {}
		Vector3(const T x, const T y, const T z) : X(x), Y(y), Z(z) {}
		Vector3(const Vector2<T> & vec, const T z) : X(vec.X), Y(vec.Y), Z(z) {}

		inline void Reset(T x, T y, T z) { X = x; Y = y; Z = z; }

		/**
		 * Dot product.
		 */
		template <typename T1>
		inline double Dot(const Vector3<T1> other) const {
			return X * other.X + Y * other.Y + Z * other.Z;
		}

		/**
		 * Cross product.
		 */
		template <typename T1, typename T2>
		inline Vector3<T1> Cross(const Vector3<T2> other) const {
			return Vector3<T1>(
				Y * other.Z - Z * other.Y,
				Z * other.X - X * other.Z,
				X * other.Y - Y * other.X);
		}

		/** Length squared. */
		inline double Length2() const { return X * X + Y * Y + Z * Z; }
		inline double Length() const { return FMath::Sqrt(Length2()); }

		Vector3<double> Normalize() const;
	};
}

namespace std {
	template <typename T>
	struct hash<utils::Vector3<T> > {
		size_t operator()(const utils::Vector3<T> & v) const {
			int64 seed = 0;
			std::hashCombine(seed, v.X);
			std::hashCombine(seed, v.Y);
			std::hashCombine(seed, v.Z);
			return seed;
		}
	};
}

#include "Algebra3.h"
