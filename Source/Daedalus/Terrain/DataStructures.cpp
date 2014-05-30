#include "Daedalus.h"
#include "DataStructures.h"

namespace utils {
	Vector3<uint64> operator * (const Vector3<uint64> & v1, const Vector3<uint64> & v2) {
		return Vector3<uint64>(v1.X * v2.X, v1.Y * v2.Y, v1.Z * v2.Z);
	}

	Vector3<int64> operator * (const Vector3<uint64> & v1, const Vector3<int64> & v2) {
		return Vector3<int64>(v1.X * v2.X, v1.Y * v2.Y, v1.Z * v2.Z);
	}

	Vector3<int64> operator * (const Vector3<int64> & v1, const Vector3<uint64> & v2) {
		return Vector3<int64>(v1.X * v2.X, v1.Y * v2.Y, v1.Z * v2.Z);
	}

	Vector3<int64> operator * (const Vector3<int64> & v1, const Vector3<int64> & v2) {
		return Vector3<int64>(v1.X * v2.X, v1.Y * v2.Y, v1.Z * v2.Z);
	}

	Vector3<uint64> operator + (const Vector3<uint64> & v1, const Vector3<uint64> & v2) {
		return Vector3<uint64>(v1.X + v2.X, v1.Y + v2.Y, v1.Z + v2.Z);
	}

	Vector3<int64> operator + (const Vector3<uint64> & v1, const Vector3<int64> & v2) {
		return Vector3<int64>(v1.X + v2.X, v1.Y + v2.Y, v1.Z + v2.Z);
	}

	Vector3<int64> operator + (const Vector3<int64> & v1, const Vector3<uint64> & v2) {
		return Vector3<int64>(v1.X + v2.X, v1.Y + v2.Y, v1.Z + v2.Z);
	}

	Vector3<int64> operator + (const Vector3<int64> & v1, const Vector3<int64> & v2) {
		return Vector3<int64>(v1.X + v2.X, v1.Y + v2.Y, v1.Z + v2.Z);
	}
}
