#pragma once

#include "Engine.h"
#include "Vector3.h"
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
	struct Tensor2 {
		std::vector<T> Data;
		uint32 Width;		// X
		uint32 Depth;		// Y

		Tensor2() : Tensor2(0, 0) {}
		Tensor2(const Vector2<uint64> & size) : Tensor2(size.X, size.Y) {}
		Tensor2(const Vector2<uint64> & size, const T & value) :
			Tensor2(size.X, size.Y, value) {}

		Tensor2(uint64 width, uint64 depth) :
			Width(width), Depth(depth), Data(width * depth) {}
		Tensor2(uint64 width, uint64 depth, const T & value) :
			Width(width), Depth(depth), Data(width * depth, value) {}

		Tensor2 & Reset(uint32 Width, uint32 Depth, const T & value) {
			this->Width = Width;
			this->Depth = Depth;
			Data.resize(Width * Depth, value);

			return *this;
		}

		const T & Get(const uint32 & x, const uint32 & y) const {
			return Data[x * Width + y];
		}

		T & Get(const uint32 & x, const uint32 & y) {
			return Data[x * Width + y];
		}

		void Set(const uint32 & x, const uint32 & y, const T & value) {
			Data[x * Width + y] = value;
		}

		void Fill(const T & value) {
			Data.assign(Width * Depth, value);
		}
	};

	template<typename T>
	struct Tensor3 {
		std::vector<T> Data;
		uint32 Width;		// X
		uint32 Depth;		// Y
		uint32 Height;		// Z

		Tensor3() : Tensor3(0, 0, 0) {}
		Tensor3(const Vector3<uint64> & size) : Tensor3(size.X, size.Y, size.Z) {}
		Tensor3(const Vector3<uint64> & size, const T & value) :
			Tensor3(size.X, size.Y, size.Z, value) {}

		Tensor3(uint64 width, uint64 depth, uint64 height) :
			Width(width), Height(height), Depth(depth), Data(width * height * depth) {}
		Tensor3(uint64 width, uint64 depth, uint64 height, const T & value) :
			Width(width), Height(height), Depth(depth), Data(width * height * depth, value) {}

		Tensor3 & Reset(uint32 Width, uint32 Height, uint32 Depth, const T & value) {
			this->Width = Width;
			this->Height = Height;
			this->Depth = Depth;
			Data.resize(Width * Height * Depth, value);

			return *this;
		}

		const T & Get(const uint32 & x, const uint32 & y, const uint32 & z) const {
			return Data[(x * Width + y) * Height + z];
		}

		T & GetMutable(const uint32 & x, const uint32 & y, const uint32 & z) {
			return Data[(x * Width + y) * Height + z];
		}

		void Set(const uint32 & x, const uint32 & y, const uint32 & z, const T & value) {
			Data[(x * Width + y) * Height + z] = value;
		}

		void Fill(const T & value) {
			Data.assign(Width * Height * Depth, value);
		}
	};
}
