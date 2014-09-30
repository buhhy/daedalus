#pragma once

#include "Algebra.h"
#include <Utilities/Algebra/DataStructures2D.h>
#include <Utilities/DataStructures.h>
#include <iostream>

namespace utils {

	/**
	 * Unary operators
	 */

	template <typename T>
	inline Vector2D<T> operator - (const Vector2D<T> & lhs) {
		return Vector2D<T>(-lhs.X, -lhs.Y);
	}

	/**
	 * Binary operators with LHS vector and RHS vector
	 */

	template <typename T>
	inline Vector2D<T> operator + (const Vector2D<T> & lhs, const Vector2D<T> & rhs) {
		return Vector2D<T>(lhs.X + rhs.X, lhs.Y + rhs.Y);
	}

	template <typename T>
	inline Vector2D<T> operator - (const Vector2D<T> & lhs, const Vector2D<T> & rhs) {
		return Vector2D<T>(lhs.X - rhs.X, lhs.Y - rhs.Y);
	}

	template <typename T>
	inline Vector2D<T> operator * (const Vector2D<T> & lhs, const Vector2D<T> & rhs) {
		return Vector2D<T>(lhs.X * rhs.X, lhs.Y * rhs.Y);
	}

	template <typename T>
	inline Vector2D<T> operator / (const Vector2D<T> & lhs, const Vector2D<T> & rhs) {
		return Vector2D<T>(lhs.X / rhs.X, lhs.Y / rhs.Y);
	}

	/**
	 * Binary operators with LHS vector RHS primitive
	 */

	template <typename T>
	inline Vector2D<T> operator + (const Vector2D<T> & lhs, const T & rhs) {
		return Vector2D<T>(lhs.X + rhs, lhs.Y + rhs);
	}

	template <typename T>
	inline Vector2D<T> operator - (const Vector2D<T> & lhs, const T & rhs) {
		return Vector2D<T>(lhs.X - rhs, lhs.Y - rhs);
	}

	template <typename T>
	inline Vector2D<T> operator * (const Vector2D<T> & lhs, const T & rhs) {
		return Vector2D<T>(lhs.X * rhs, lhs.Y * rhs);
	}

	template <typename T>
	inline Vector2D<T> operator / (const Vector2D<T> & lhs, const T & rhs) {
		return Vector2D<T>(lhs.X / rhs, lhs.Y / rhs);
	}

	/**
	 * Comparison operators
	 */

	template <typename T>
	inline bool operator == (const Vector2D<T> & lhs, const Vector2D<T> & rhs) {
		return lhs.X == rhs.X && lhs.Y == rhs.Y;
	}

	template <typename T>
	inline bool operator != (const Vector2D<T> & lhs, const Vector2D<T> & rhs) {
		return lhs.X != rhs.X || lhs.Y != rhs.Y;
	}

	template <typename T>
	inline bool operator < (const Vector2D<T> & lhs, const Vector2D<T> & rhs) {
		return lhs.X == rhs.X ? lhs.Y < rhs.Y : lhs.X < rhs.X;
	}

	template <typename T>
	std::ostream & operator << (std::ostream & os, const Vector2D<T> & vec) {
		os << '(' << vec.X << ", " << vec.Y << ')';
		return os;
	}

	/********************************************************************************
	 * Other calculations in 2D
	 ********************************************************************************/

	Circle2D CalculateCircumcircle(
		const Vector2D<> & A, const Vector2D<> & B, const Vector2D<> & C);

	/**
	 * Returns -1: outside circumcircle, 0: on perimeter of circumcircle, 1: inside circumcircle
	 */
	Int8 IsWithinCircumcircle(const Vector2D<> & point, const Circle2D & circle);

	/**
	 * Returns -1: >180 degrees, 0: straight, 1: < 180
	 */
	Int8 FindWinding(const Vector2D<> & p1, const Vector2D<> & pivot, const Vector2D<> & p3);
	double FindAngle(const Vector2D<> & v1, const Vector2D<> & v2);

	
	Vector2D<> projectVector2D(const Vector2D<> projection, const Vector2D<> target);
	void gramSchmidt2D(Vector2D<> & x, Vector2D<> & y);

	/**
	 * Builds a basis around the input vector. The input is always assumed to be X and the Y
	 * vector is built around that.
	 */
	void buildBasis2D(Vector2D<> & input, Vector2D<> & y);
}
