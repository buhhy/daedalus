#pragma once

#include "Algebra.h"
#include <Utilities/Algebra/Tensor3D.h>
#include <Utilities/Algebra/DataStructures2D.h>
#include <Utilities/Algebra/DataStructures3D.h>
#include <Utilities/DataStructures.h>

#include <iostream>

namespace utils {
	/********************
	 * Vector3D Operators
	 ********************/

	/**
	 * Unary operators
	 */

	template<typename T>
	inline Vector3D<T> operator - (const Vector3D<T> & lhs) {
		return Vector3D<T>(-lhs.X, -lhs.Y, -lhs.Z);
	}

	/**
	 * Binary operators with LHS vector and RHS vector
	 */

	template<typename T>
	inline Vector3D<T> operator + (const Vector3D<T> & lhs, const Vector3D<T> & rhs) {
		return Vector3D<T>(lhs.X + rhs.X, lhs.Y + rhs.Y, lhs.Z + rhs.Z);
	}

	template<typename T>
	inline Vector3D<T> operator - (const Vector3D<T> & lhs, const Vector3D<T> & rhs) {
		return Vector3D<T>(lhs.X - rhs.X, lhs.Y - rhs.Y, lhs.Z - rhs.Z);
	}

	template<typename T>
	inline Vector3D<T> operator * (const Vector3D<T> & lhs, const Vector3D<T> & rhs) {
		return Vector3D<T>(lhs.X * rhs.X, lhs.Y * rhs.Y, lhs.Z * rhs.Z);
	}

	template<typename T>
	inline Vector3D<T> operator / (const Vector3D<T> & lhs, const Vector3D<T> & rhs) {
		return Vector3D<T>(lhs.X / rhs.X, lhs.Y / rhs.Y, lhs.Z / rhs.Z);
	}

	/**
	 * Binary operators with LHS vector RHS primitive
	 */

	template<typename T>
	inline Vector3D<T> operator + (const Vector3D<T> & lhs, const T & rhs) {
		return Vector3D<T>(lhs.X + rhs, lhs.Y + rhs, lhs.Z + rhs);
	}

	template<typename T>
	inline Vector3D<T> operator - (const Vector3D<T> & lhs, const T & rhs) {
		return Vector3D<T>(lhs.X - rhs, lhs.Y - rhs, lhs.Z - rhs);
	}

	template<typename T>
	inline Vector3D<T> operator * (const Vector3D<T> & lhs, const T & rhs) {
		return Vector3D<T>(lhs.X * rhs, lhs.Y * rhs, lhs.Z * rhs);
	}

	template<typename T>
	inline Vector3D<T> operator / (const Vector3D<T> & lhs, const T & rhs) {
		return Vector3D<T>(lhs.X / rhs, lhs.Y / rhs, lhs.Z / rhs);
	}

	/**
	 * Comparison operators for vectors
	 */

	template<typename T>
	inline bool operator == (const Vector3D<T> & lhs, const Vector3D<T> & rhs) {
		return lhs.X == rhs.X && lhs.Y == rhs.Y && lhs.Z == rhs.Z;
	}

	template<typename T>
	inline bool operator != (const Vector3D<T> & lhs, const Vector3D<T> & rhs) {
		return lhs.X != rhs.X && lhs.Y != rhs.Y && lhs.Z != rhs.Z;
	}

	template <typename T>
	std::ostream& operator << (std::ostream &output, const Vector3D<T> vec) {
		return output << '(' << vec.X << ", " << vec.Y << ", " << vec.Z << ')';
	}



	/********************
	 * Matrix4D Operators
	 ********************/
	
	template <typename T>
	inline Matrix4D<T> operator * (const Matrix4D<T> & a, const Matrix4D<T> & b) {
		Matrix4D<T> ret;

		for (Uint32 i = 0; i < 4; ++i) {
			const T * row = a[i];

			for (Uint32 j = 0; j < 4; ++j) {
				ret[i][j] =
					row[0] * b[0][j] + row[1] * b[1][j] + 
					row[2] * b[2][j] + row[3] * b[3][j];
			}
		}

		return ret;
	}
	
	template <typename T>
	inline Vector4D<T> operator * (const Matrix4D<T> & mat, const Vector4D<T> & vec) {
		return Vector4D<T>(
			vec.X * mat[0][0] + vec.Y * mat[0][1] + vec.Z * mat[0][2] + vec.W * mat[0][3],
			vec.X * mat[1][0] + vec.Y * mat[1][1] + vec.Z * mat[1][2] + vec.W * mat[1][3],
			vec.X * mat[2][0] + vec.Y * mat[2][1] + vec.Z * mat[2][2] + vec.W * mat[2][3],
			vec.X * mat[3][0] + vec.Y * mat[3][1] + vec.Z * mat[3][2] + vec.W * mat[3][3]);
	}
	
	template <typename T>
	inline Vector3D<T> operator * (const Matrix4D<T> & mat, const Vector3D<T> & vec) {
		return Vector3D<T>(
			vec.X * mat[0][0] + vec.Y * mat[0][1] + vec.Z * mat[0][2] + mat[0][3],
			vec.X * mat[1][0] + vec.Y * mat[1][1] + vec.Z * mat[1][2] + mat[1][3],
			vec.X * mat[2][0] + vec.Y * mat[2][1] + vec.Z * mat[2][2] + mat[2][3]);
	}
	
	inline Ray3D operator * (const Matrix4D<> & mat, const Ray3D & ray) {
		return Ray3D(mat * ray.Origin, (GetRotationMatrixFrom(mat) * ray.Origin).Normalize());
	}

	

	/********************
	 * Misc operations
	 ********************/
	
	Vector3D<> Project(const Vector3D<> projection, const Vector3D<> target);

	void GramSchmidt(Vector3D<> & x, Vector3D<> & y, Vector3D<> & z);

	/**
	 * Builds a basis around the input vector. The input is always assumed to be X and the Y
	 * and Z vectors are built around that. Z will be up by default, unless the input is up,
	 * in which case Z will be set to right.
	 */
	void BuildBasis(Vector3D<> & input, Vector3D<> & y, Vector3D<> & z);
}
