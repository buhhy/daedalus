#include <Daedalus.h>
#include <Utilities/DataStructures.h>

namespace utils {
	template <typename T>
	inline T Abs(T value) { return value > 0.0 ? value : -value; }

	inline uint16_t Abs(uint16_t value) { return value; }
	inline uint32_t Abs(uint32_t value) { return value; }
	inline uint64_t Abs(uint64_t value) { return value; }

	template <typename T>
	Vector3<double> Vector3<T>::Normalize() const {
		double denom = 1.0;
		T x = Abs(X);
		T y = Abs(Y);
		T z = Abs(Z);

		if (x > y) {
			if (x > z) { /* x > z, y */
				if (1.0 + x > 1.0) {
					y = y / x;
					z = z / x;
					denom = 1.0 / (x * sqrt(1.0 + y*y + z*z));
				}
			} else { /* z > x > y */
				if (1.0 + z > 1.0) {
					y = y / z;
					x = x / z;
					denom = 1.0 / (z * sqrt(1.0 + y*y + x*x));
				}
			}
		} else {
			if (y > z) { /* y > x, z */
				if (1.0 + y > 1.0) {
					z = z / y;
					x = x / y;
					denom = 1.0 / (y * sqrt(1.0 + z*z + x*x));
				}
			} else { /* x < y < z */
				if (1.0 + z > 1.0) {
					y = y / z;
					x = x / z;
					denom = 1.0 / (z * sqrt(1.0 + y*y + x*x));
				}
			}
		}

		// Vector isn't (0, 0, 0)
		if (1.0 + x + y + z > 1.0)
			return Vector3<double>((double) X * denom, (double) Y * denom, (double) Z * denom);

		return Vector3<double>((double) X, (double) Y, (double) Z);
	}
	
	template <typename T>
	Vector3<T> & Vector3<T>::operator = (const Vector4<T> & other) {
		// Assignment swap principle
		T x = other.X, y = other.Y, z = other.Z;
		Reset(x, y, z);
		return *this;
	}

	template Vector3<float>;
	template Vector3<double>;
	template Vector3<int>;
	template Vector3<int16_t>;
	template Vector3<int32_t>;
	template Vector3<int64_t>;
	template Vector3<uint16_t>;
	template Vector3<uint32_t>;
	template Vector3<uint64_t>;
}
