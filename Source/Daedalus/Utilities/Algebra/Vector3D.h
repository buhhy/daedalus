#pragma once

#include <Utilities/DataStructures.h>
#include <Utilities/Algebra/Vector2D.h>
#include <Utilities/Algebra/Algebra.h>

#include <cassert>
#include <functional>
#include <sstream>

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
		Vector3D(const T & v) : X(v), Y(v), Z(v) {}
		Vector3D(const T & x, const T & y, const T & z) : X(x), Y(y), Z(z) {}
		Vector3D(const Vector2D<T> & vec) : Vector3D(vec.X, vec.Y, 0) {}
		Vector3D(const Vector2D<T> & vec, const T z) : Vector3D(vec.X, vec.Y, z) {}

		bool IsBoundedBy(
			const T min, const T max,
			const bool isMinIncluded = true,
			const bool isMaxIncluded = false
		) const {
			if (isMinIncluded && (X < min || Y < min || Z < min) ||
				!isMinIncluded && (X <= min || Y <= min || Z <= min)) return false;
			
			if (isMaxIncluded && (X > max || Y > max || Z > max) ||
				!isMaxIncluded && (X >= max || Y >= max || Z >= max)) return false;

			return true;
		}

		bool IsBoundedBy(
			const Vector3D<T> & min, const Vector3D<T> & max,
			const bool isMinIncluded = true,
			const bool isMaxIncluded = false
		) const {
			if (isMinIncluded && (X < min.X || Y < min.Z || Z < min.Z) ||
				!isMinIncluded && (X <= min.X || Y <= min.Y || Z <= min.Z)) return false;
			
			if (isMaxIncluded && (X > max.X || Y > max.Y || Z > max.Z) ||
				!isMaxIncluded && (X >= max.X || Y >= max.Y || Z >= max.Z)) return false;

			return true;
		}

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

		inline bool IsNormal() const { return std::abs(Length2() - 1) <= FLOAT_ERROR; }

		inline Vector3D<> Normalize() const {
			const double length2 = Length2();
			if (EEq(length2, 1))
				return Vector3D<>(X, Y, Z);
			const double length = std::sqrt(length2);
			return Vector3D<>((double) X / length, (double) Y / length, (double) Z / length);
		}

		inline Vector3D<T> & operator = (const Vector3D<T> & other) {
			// Assignment swap principle
			T x = other.X, y = other.Y, z = other.Z;
			Reset(x, y, z);
			return *this;
		}

		inline Vector2D<T> Truncate() const { return Vector2D<T>(X, Y); }

		template <typename T1>
		inline Vector3D<T1> Cast() const {
			return Vector3D<T1>((T1) this->X, (T1) this->Y, (T1) this->Z);
		}

		inline T & operator [] (const Uint64 index) {
			switch (index) {
			case 0: return X;
			case 1: return Y;
			case 2: return Z;
			default:
				std::stringstream ss;
				ss << "Vector3D::[]: Invalid index value `" << index << "`.";
				throw StringException(ss.str());
			}
		}

		inline const T & operator [] (const Uint64 index) const {
			switch (index) {
			case 0: return X;
			case 1: return Y;
			case 2: return Z;
			default:
				std::stringstream ss;
				ss << "Vector3D::[]: Invalid index value `" << index << "`.";
				throw StringException(ss.str());
			}
		}

		inline Vector3D<T> & operator += (const Vector3D<T> & rhs) {
			X += rhs.X; Y += rhs.Y; Z += rhs.Z;
			return *this;
		}
		inline Vector3D<T> & operator -= (const Vector3D<T> & rhs) {
			X -= rhs.X; Y -= rhs.Y; Z -= rhs.Z;
			return *this;
		}
		inline Vector3D<T> & operator *= (const Vector3D<T> & rhs) {
			X *= rhs.X; Y *= rhs.Y; Z *= rhs.Z;
			return *this;
		}
		inline Vector3D<T> & operator /= (const Vector3D<T> & rhs) {
			X /= rhs.X; Y /= rhs.Y; Z /= rhs.Z;
			return *this;
		}

		inline Vector3D<T> & operator += (const T & rhs) {
			X += rhs; Y += rhs; Z += rhs;
			return *this;
		}
		inline Vector3D<T> & operator -= (const T & rhs) {
			X -= rhs; Y -= rhs; Z -= rhs;
			return *this;
		}
		inline Vector3D<T> & operator *= (const T & rhs) {
			X *= rhs; Y *= rhs; Z *= rhs;
			return *this;
		}
		inline Vector3D<T> & operator /= (const T & rhs) {
			X /= rhs; Y /= rhs; Z /= rhs;
			return *this;
		}
	};
}

namespace std {
	template <typename T>
	struct hash<utils::Vector3D<T>> {
		hash<T> hasher;
		size_t operator()(const utils::Vector3D<T> & v) const {
			Int64 seed = 0;
			std::hashCombine(seed, v.X, hasher);
			std::hashCombine(seed, v.Y, hasher);
			std::hashCombine(seed, v.Z, hasher);
			return (unsigned) seed;
		}
	};
}
