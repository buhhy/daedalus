#pragma once

#include "Algebra.h"
#include <Utilities/DataStructures.h>

namespace utils {
	/********************
	 * Vector3 Operators
	 ********************/

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
	 * Comparison operators for vectors
	 */

	template<typename T>
	inline bool operator == (const Vector3<T> & lhs, const Vector3<T> & rhs) {
		return lhs.X == rhs.X && lhs.Y == rhs.Y && lhs.Z == rhs.Z;
	}

	/********************
	 * Matrix4 Operators
	 ********************/
	
	template <typename T>
	inline Matrix4<T> operator * (const Matrix4<T> & a, const Matrix4<T> & b) {
		Matrix4<T> ret;

		for (size_t i = 0; i < 4; ++i) {
			const T * row = a[i];

			for (size_t j = 0; j < 4; ++j) {
				ret[i][j] =
					row[0] * b[0][j] + row[1] * b[1][j] + 
					row[2] * b[2][j] + row[3] * b[3][j];
			}
		}

		return ret;
	}
	
	template <typename T>
	inline Vector4<T> operator * (const Matrix4<T> & mat, const Vector4<T> & vec) {
		return Vector4<T>(
			vec.X * mat[0][0] + vec.Y * mat[0][1] + vec.Z * mat[0][2] + vec.W * mat[0][3],
			vec.X * mat[1][0] + vec.Y * mat[1][1] + vec.Z * mat[1][2] + vec.W * mat[1][3],
			vec.X * mat[2][0] + vec.Y * mat[2][1] + vec.Z * mat[2][2] + vec.W * mat[2][3],
			vec.X * mat[3][0] + vec.Y * mat[3][1] + vec.Z * mat[3][2] + vec.W * mat[3][3]);
	}

	

	/********************
	 * Misc operations
	 ********************/
	
	Vector3<> Project(const Vector3<> projection, const Vector3<> target);

	void GramSchmidt(Vector3<> & x, Vector3<> & y, Vector3<> & z);

	/**
	 * Builds a basis around the input vector. The input is always assumed to be X and the Y
	 * and Z vectors are built around that. Z will be up by default, unless the input is up,
	 * in which case Z will be set to right.
	 */
	void BuildBasis(Vector3<> & input, Vector3<> & y, Vector3<> & z);
}
