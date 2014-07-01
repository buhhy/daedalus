#pragma once

#include "Vector3.h"

#include <Utilities/Algebra/Algebra.h>

namespace utils {
	/**
	 * This xyzw vector is used in 3D space for affine transforms.
	 */
	template <typename T = double>
	struct Vector4 {
		T X;
		T Y;
		T Z;
		T W;

		Vector4() {}
		Vector4(const T x, const T y, const T z, const T w) : X(x), Y(y), Z(z), W(w) {}
		Vector4(const Vector3<T> & vec, const T w = 0) : Vector4(vec.X, vec.Y, vec.Z, w) {}

		inline void Reset(T x, T y, T z, T w) { X = x; Y = y; Z = z; W = w; }

		/**
		 * Dot product.
		 */
		template <typename T1>
		inline double Dot(const Vector4<T1> other) const {
			return X * other.X + Y * other.Y + Z * other.Z + W * other.W;
		}

		/**
		 * Cross product. Since this operation is meant to operate in a 3D space, we use the
		 * same cross product algorithm as Vector3.
		 */
		template <typename T1, typename T2>
		inline Vector4<T1> Cross(const Vector4<T2> other) const {
			return Vector4<T1>(
				Y * other.Z - Z * other.Y,
				Z * other.X - X * other.Z,
				X * other.Y - Y * other.X);
		}

		/* Length squared. */
		inline double Length2() const { return X * X + Y * Y + Z * Z; }
		inline double Length() const { return std::abs(Length2()); }
	};
}
