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

	/**
	 * Other calculations in 2D
	 */

	Circle2D CalculateCircumcircle(
		const Vector2<> & A, const Vector2<> & B, const Vector2<> & C);

	/**
	 * Returns -1: outside circumcircle, 0: on perimeter of circumcircle, 1: inside circumcircle
	 */
	int8_t IsWithinCircumcircle(const Vector2<> & point, const Circle2D & circle);

	/**
	 * Returns -1: >180 degrees, 0: straight, 1: < 180
	 */
	int8_t FindWinding(const Vector2<> & p1, const Vector2<> & pivot, const Vector2<> & p3);
	double FindAngle(const Vector2<> & v1, const Vector2<> & v2);
}
