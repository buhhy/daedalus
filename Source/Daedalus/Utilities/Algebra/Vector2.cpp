#include <Daedalus.h>
#include <Utilities/DataStructures.h>

namespace utils {
	template <typename T>
	Vector2<T>::Vector2(const Vector3<T> & copy): Vector2(copy.X, copy.Y) {}

	template <typename T>
	inline T Abs(T value) { return value > 0.0 ? value : -value; }

	inline uint16_t Abs(uint16_t value) { return value; }
	inline uint32_t Abs(uint32_t value) { return value; }
	inline uint64_t Abs(uint64_t value) { return value; }

	template <typename T>
	Vector2<double> Vector2<T>::Normalize() const {
		double denom = 1.0;
		T x = Abs(X);
		T y = Abs(Y);

		if (x > y) { /* x > y */
			if (1.0 + x > 1.0) {
				y = y / x;
				denom = 1.0 / (x * sqrt(1.0 + y*y));
			}
		} else { /* y > x */
			if (1.0 + y > 1.0) {
				x = x / y;
				denom = 1.0 / (y * sqrt(1.0 + x*x));
			}
		}

		// Vector isn't (0, 0)
		if (1.0 + x + y > 1.0)
			return Vector2<double>((double) X * denom, (double) Y * denom);

		return Vector2<double>((double) X, (double) Y);
	}

	// Explicit instantiations
	template struct Vector2<float>;
	template struct Vector2<double>;
	template struct Vector2<int16_t>;
	template struct Vector2<int32_t>;
	template struct Vector2<int64_t>;
	template struct Vector2<uint16_t>;
	template struct Vector2<uint32_t>;
	template struct Vector2<uint64_t>;
}
