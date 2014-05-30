#pragma once

#include "Engine.h"
#include <vector>

namespace utils {
	struct Triangle {
		FVector points[3];

		Triangle(const FVector & p1, const FVector & p2, const FVector & p3) {
			points[0] = p1;
			points[1] = p2;
			points[2] = p3;
		}
	};

	struct GridCell {
		float values[8];
		FVector points[8];

		void Initialize(
			const float blf, const float tlf,
			const float blb, const float tlb,
			const float brf, const float trf,
			const float brb, const float trb
			) {
			values[0] = blf; values[1] = brf; values[2] = brb; values[3] = blb;
			values[4] = tlf; values[5] = trf; values[6] = trb; values[7] = tlb;
			points[0] = FVector(0, 0, 0);
			points[1] = FVector(1, 0, 0);
			points[2] = FVector(1, 1, 0);
			points[3] = FVector(0, 1, 0);
			points[4] = FVector(0, 0, 1);
			points[5] = FVector(1, 0, 1);
			points[6] = FVector(1, 1, 1);
			points[7] = FVector(0, 1, 1);
		}
	};

	template<typename T>
	struct Vector3 {
		T X;
		T Y;
		T Z;

		Vector3() {}
		Vector3(T X, T Y, T Z) {
			Init(X, Y, Z);
		}

		Vector3 & Init(T X, T Y, T Z) {
			this->X = X;
			this->Y = Y;
			this->Z = Z;

			return *this;
		}
	};

	Vector3<uint64> operator * (const Vector3<uint64> & v1, const Vector3<uint64> & v2);
	Vector3<int64> operator * (const Vector3<uint64> & v1, const Vector3<int64> & v2);
	Vector3<int64> operator * (const Vector3<int64> & v1, const Vector3<uint64> & v2);
	Vector3<int64> operator * (const Vector3<int64> & v1, const Vector3<int64> & v2);
	Vector3<uint64> operator + (const Vector3<uint64> & v1, const Vector3<uint64> & v2);
	Vector3<int64> operator + (const Vector3<uint64> & v1, const Vector3<int64> & v2);
	Vector3<int64> operator + (const Vector3<int64> & v1, const Vector3<uint64> & v2);
	Vector3<int64> operator + (const Vector3<int64> & v1, const Vector3<int64> & v2);

	template<typename T>
	struct Tensor3 {
		std::vector<T> Data;
		uint32 Width;		// X
		uint32 Depth;		// Y
		uint32 Height;		// Z

		Tensor3() : Width(0), Height(0), Depth(0) {}

		Tensor3 & Init(uint32 Width, uint32 Height, uint32 Depth) {
			this->Width = Width;
			this->Height = Height;
			this->Depth = Depth;
			Data.resize(Width * Height * Depth, 0);

			return *this;
		}

		const T & Get(const uint32 & x, const uint32 & y, const uint32 & z) const {
			return Data[(x * Width + y) * Height + z];
		}

		void Set(const uint32 & x, const uint32 & y, const uint32 & z, T value) {
			Data[(x * Width + y) * Height + z] = value;
		}
	};
}
