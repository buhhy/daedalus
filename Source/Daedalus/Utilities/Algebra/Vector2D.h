#pragma once

#include <Utilities/Algebra/Algebra.h>

#include <cmath>
#include <functional>

namespace utils {
	template <typename T>
	struct Vector3D;

	template <typename T = double>
	struct Vector2D {
		T X;
		T Y;

		Vector2D() {}
		Vector2D(const T & v) : X(v), Y(v) {}
		Vector2D(const T & x, const T & y) : X(x), Y(y) {}
		Vector2D(const Vector2D<T> & copy) : Vector2D(copy.X, copy.Y) {}

		inline void Reset(const T & x, const T & y) { X = x; Y = y; }
		inline void Reset(const Vector2D<T> & other) { Reset(other.X, other.Y); }

		/** Dot product. */
		template <typename T1>
		inline double Dot(const Vector2D<T1> other) const {
			return X * other.X + Y * other.Y;
		}

		/**
		 * Takes the determinant of the 2 vectors, effectively finding the winding order of
		 * the 2 vectors.
		 * | x1 y1 |
		 * | x2 y2 |
		 */
		inline T Determinant(const Vector2D<T> other) const {
			return X * other.Y - Y * other.X;
		}

		/** Length squared. */
		inline double Length2() const { return (double) X * X + (double) Y * Y; }
		inline double Length() const { return std::sqrt(Length2()); }

		template <typename T1>
		inline Vector2D<T1> Cast() const { return Vector2D<T1>((T1) this->X, (T1) this->Y); }

		inline Vector2D<T> & operator += (const Vector2D<T> & rhs) { X += rhs.X; Y += rhs.Y; return *this; }
		inline Vector2D<T> & operator -= (const Vector2D<T> & rhs) { X -= rhs.X; Y -= rhs.Y; return *this; }
		inline Vector2D<T> & operator *= (const Vector2D<T> & rhs) { X *= rhs.X; Y *= rhs.Y; return *this; }
		inline Vector2D<T> & operator /= (const Vector2D<T> & rhs) { X /= rhs.X; Y /= rhs.Y; return *this; }

		inline Vector2D<T> & operator += (const T & rhs) { X += rhs; Y += rhs; return *this; }
		inline Vector2D<T> & operator -= (const T & rhs) { X -= rhs; Y -= rhs; return *this; }
		inline Vector2D<T> & operator *= (const T & rhs) { X *= rhs; Y *= rhs; return *this; }
		inline Vector2D<T> & operator /= (const T & rhs) { X /= rhs; Y /= rhs; return *this; }

		inline Vector2D<> Normalize() const {
			double length = Length();
			return Vector2D<>((double) X / length, (double) Y / length);
		}
	};
}

namespace std {
	template <typename T>
	struct hash<utils::Vector2D<T>> {
		hash<T> hasher;
		size_t operator()(const utils::Vector2D<T> & v) const {
			Int64 seed = 0;
			std::hashCombine(seed, v.X, hasher);
			std::hashCombine(seed, v.Y, hasher);
			return (unsigned) seed;
		}
	};
}