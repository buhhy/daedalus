#pragma once

#include "Algebra.h"

namespace utils {

	/**
	 * Unary operators
	 */

	template<typename T>
	inline Vector2<T> operator - (const Vector2<T> & lhs) {
		return Vector2<T>(-lhs.X, -lhs.Y);
	}

	/**
	 * Binary operators with LHS vector and RHS vector
	 */

	template<typename T>
	inline Vector2<T> operator + (const Vector2<T> & lhs, const Vector2<T> & rhs) {
		return Vector2<T>(lhs.X + rhs.X, lhs.Y + rhs.Y);
	}

	template<typename T>
	inline Vector2<T> operator - (const Vector2<T> & lhs, const Vector2<T> & rhs) {
		return Vector2<T>(lhs.X - rhs.X, lhs.Y - rhs.Y);
	}

	template<typename T>
	inline Vector2<T> operator * (const Vector2<T> & lhs, const Vector2<T> & rhs) {
		return Vector2<T>(lhs.X * rhs.X, lhs.Y * rhs.Y);
	}

	template<typename T>
	inline Vector2<T> operator / (const Vector2<T> & lhs, const Vector2<T> & rhs) {
		return Vector2<T>(lhs.X / rhs.X, lhs.Y / rhs.Y);
	}

	/**
	 * Binary operators with LHS vector RHS primitive
	 */

	template<typename T>
	inline Vector2<T> operator + (const Vector2<T> & lhs, const T & rhs) {
		return Vector2<T>(lhs.X + rhs, lhs.Y + rhs);
	}

	template<typename T>
	inline Vector2<T> operator - (const Vector2<T> & lhs, const T & rhs) {
		return Vector2<T>(lhs.X - rhs, lhs.Y - rhs);
	}

	template<typename T>
	inline Vector2<T> operator * (const Vector2<T> & lhs, const T & rhs) {
		return Vector2<T>(lhs.X * rhs, lhs.Y * rhs);
	}

	template<typename T>
	inline Vector2<T> operator / (const Vector2<T> & lhs, const T & rhs) {
		return Vector2<T>(lhs.X / rhs, lhs.Y / rhs);
	}

	/**
	 * Comparison operators
	 */

	template<typename T>
	inline bool operator == (const Vector2<T> & lhs, const Vector2<T> & rhs) {
		return lhs.X == rhs.X && lhs.Y == rhs.Y;
	}

	template<typename T>
	inline bool operator < (const Vector2<T> & lhs, const Vector2<T> & rhs) {
		return lhs.X == rhs.X ? lhs.Y < rhs.Y : lhs.X < rhs.X;
	}
}
