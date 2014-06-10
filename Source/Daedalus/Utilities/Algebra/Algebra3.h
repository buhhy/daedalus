#pragma once

#include "Algebra.h"

namespace utils {
	/**
	 * Unary operators
	 */

	template<typename T>
	inline Vector3<T> operator - (const Vector3<T> & lhs) {
		return Vector3<T>(-lhs.X, -lhs.Y, -lhs.Z);
	}

	/**
	 * Binary operators with LHS vector and RHS vector
	 */

	template<typename T>
	inline Vector3<T> operator + (const Vector3<T> & lhs, const Vector3<T> & rhs) {
		return Vector3<T>(lhs.X + rhs.X, lhs.Y + rhs.Y, lhs.Z + rhs.Z);
	}

	template<typename T>
	inline Vector3<T> operator - (const Vector3<T> & lhs, const Vector3<T> & rhs) {
		return Vector3<T>(lhs.X - rhs.X, lhs.Y - rhs.Y, lhs.Z - rhs.Z);
	}

	template<typename T>
	inline Vector3<T> operator * (const Vector3<T> & lhs, const Vector3<T> & rhs) {
		return Vector3<T>(lhs.X * rhs.X, lhs.Y * rhs.Y, lhs.Z * rhs.Z);
	}

	template<typename T>
	inline Vector3<T> operator / (const Vector3<T> & lhs, const Vector3<T> & rhs) {
		return Vector3<T>(lhs.X / rhs.X, lhs.Y / rhs.Y, lhs.Z / rhs.Z);
	}

	/**
	 * Binary operators with LHS vector RHS primitive
	 */

	template<typename T>
	inline Vector3<T> operator + (const Vector3<T> & lhs, const T & rhs) {
		return Vector3<T>(lhs.X + rhs, lhs.Y + rhs, lhs.Z + rhs);
	}

	template<typename T>
	inline Vector3<T> operator - (const Vector3<T> & lhs, const T & rhs) {
		return Vector3<T>(lhs.X - rhs, lhs.Y - rhs, lhs.Z - rhs);
	}

	template<typename T>
	inline Vector3<T> operator * (const Vector3<T> & lhs, const T & rhs) {
		return Vector3<T>(lhs.X * rhs, lhs.Y * rhs, lhs.Z * rhs);
	}

	template<typename T>
	inline Vector3<T> operator / (const Vector3<T> & lhs, const T & rhs) {
		return Vector3<T>(lhs.X / rhs, lhs.Y / rhs, lhs.Z / rhs);
	}

	/**
	 * Comparison operators
	 */

	template<typename T>
	inline bool operator == (const Vector3<T> & lhs, const Vector3<T> & rhs) {
		return lhs.X == rhs.X && lhs.Y == rhs.Y && lhs.Z == rhs.Z;
	}
}
