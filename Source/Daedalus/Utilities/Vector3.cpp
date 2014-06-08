#include "Daedalus.h"
#include "Vector3.h"

namespace utils {
	template <typename T>
	inline T Abs(T value) { return value > 0.0 ? value : -value; }

	inline uint16 Abs(uint16 value) { return value; }
	inline uint32 Abs(uint32 value) { return value; }
	inline uint64 Abs(uint64 value) { return value; }

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

		if (1.0 + x + y + z > 1.0)
			return *this * denom;

		return *this * 1.0;
	}

	template Vector3<float>;
	template Vector3<double>;
	template Vector3<int>;
	template Vector3<int16>;
	template Vector3<int32>;
	template Vector3<int64>;
	template Vector3<uint16>;
	template Vector3<uint32>;
	template Vector3<uint64>;
}
