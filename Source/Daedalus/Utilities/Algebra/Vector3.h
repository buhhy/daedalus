#pragma once

#include <Utilities/Algebra/Vector2.h>
#include <Utilities/Algebra/Algebra.h>
#include <functional>

namespace utils {
	template <typename T>
	struct Vector4;

	/**
	 * A standard XYZ vector that can be used as a hashmap key.
	 */
	template <typename T = double>
	struct Vector3 {
		T X, Y, Z;

		Vector3() {}
		Vector3(const T x, const T y, const T z) : X(x), Y(y), Z(z) {}
		Vector3(const Vector2<T> & vec, const T z) : Vector3(vec.X, vec.Y, z) {}

		/**
		 * Dot product.
		 */
		inline T Dot(const Vector3<T> other) const {
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
		
		inline void Reset(T x, T y, T z) { X = x; Y = y; Z = z; }
		inline void Reset(const Vector2<T> & vec, T z) { X = vec.X; Y = vec.Y; Z = z; }

		/** Length squared. */
		inline T Length2() const { return X * X + Y * Y + Z * Z; }
		inline double Length() const { return std::sqrt(Length2()); }

		inline Vector3<> Normalize() const {
			double length = Length();
			return Vector3<>(
				(double) X / length, (double) Y / length, (double) Z / length);
		}

		inline Vector3<T> & operator = (const Vector3<T> & other) {
			// Assignment swap principle
			T x = other.X, y = other.Y, z = other.Z;
			Reset(x, y, z);
			return *this;
		}

		inline Vector2<T> Truncate() const { return Vector2<T>(X, Y); }
	};
}

namespace std {
	template <typename T>
	struct hash<utils::Vector3<T> > {
		size_t operator()(const utils::Vector3<T> & v) const {
			Int64 seed = 0;
			std::hashCombine(seed, v.X);
			std::hashCombine(seed, v.Y);
			std::hashCombine(seed, v.Z);
			return (unsigned) seed;
		}
	};
}
