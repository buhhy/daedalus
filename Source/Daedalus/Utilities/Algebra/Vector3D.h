#pragma once

#include <Utilities/Algebra/Vector2D.h>
#include <Utilities/Algebra/Algebra.h>
#include <functional>

namespace utils {
	template <typename T>
	struct Vector4D;

	/**
	 * A standard XYZ vector that can be used as a hashmap key.
	 */
	template <typename T = double>
	struct Vector3D {
		T X, Y, Z;

		Vector3D() {}
		Vector3D(const T x, const T y, const T z) : X(x), Y(y), Z(z) {}
		Vector3D(const Vector2D<T> & vec, const T z) : Vector3D(vec.X, vec.Y, z) {}

		/**
		 * Dot product.
		 */
		inline T Dot(const Vector3D<T> other) const {
			return X * other.X + Y * other.Y + Z * other.Z;
		}

		/**
		 * Cross product.
		 */
		template <typename T1, typename T2>
		inline Vector3D<T1> Cross(const Vector3D<T2> other) const {
			return Vector3D<T1>(
				Y * other.Z - Z * other.Y,
				Z * other.X - X * other.Z,
				X * other.Y - Y * other.X);
		}
		
		inline void Reset(T x, T y, T z) { X = x; Y = y; Z = z; }
		inline void Reset(const Vector2D<T> & vec, T z) { X = vec.X; Y = vec.Y; Z = z; }

		/** Length squared. */
		inline T Length2() const { return X * X + Y * Y + Z * Z; }
		inline double Length() const { return std::sqrt(Length2()); }

		inline Vector3D<> Normalize() const {
			double length = Length();
			return Vector3D<>(
				(double) X / length, (double) Y / length, (double) Z / length);
		}

		inline Vector3D<T> & operator = (const Vector3D<T> & other) {
			// Assignment swap principle
			T x = other.X, y = other.Y, z = other.Z;
			Reset(x, y, z);
			return *this;
		}

		inline Vector2D<T> Truncate() const { return Vector2D<T>(X, Y); }
	};
}

namespace std {
	template <typename T>
	struct hash<utils::Vector3D<T> > {
		size_t operator()(const utils::Vector3D<T> & v) const {
			Int64 seed = 0;
			std::hashCombine(seed, v.X);
			std::hashCombine(seed, v.Y);
			std::hashCombine(seed, v.Z);
			return (unsigned) seed;
		}
	};
}
