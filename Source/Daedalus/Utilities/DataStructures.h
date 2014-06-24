#pragma once

#include "Engine.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4.h"
#include <vector>

namespace utils {
	// Equivalent to the Scala Option construct which can be a Some or None value
	template <typename T>
	struct Option {
	private:
		TSharedPtr<T> Value;
	public:
		Option() : Value(NULL) {}
		Option(const T & value) : Value(new T(value)) {}
		bool IsValid() const { return Value->IsValid(); }
		T & Get() { return *Value; }
		const T & Get() const { return *Value; }
	};
	
	template <typename T>
	inline Option<T> None() { return Option<T>(); }
	
	template <typename T>
	inline Option<T> Some(const T & value) { return Option<T>(value); }

	struct Circle2D {
		Vector2<> Center;
		float Radius;

		Circle2D(const Vector2<> center, const float radius) : Center(center), Radius(radius) {}
		Circle2D() : Circle2D({ 0, 0 }, 0) {}
	};

	struct Triangle {
		Vector3<> Point1, Point2, Point3;

		Triangle(
			const Vector3<> & p1,
			const Vector3<> & p2,
			const Vector3<> & p3
		) : Point1(p1), Point2(p2), Point3(p3) {}
	};

	// TODO: get rid of this
	struct GridCell {
		float values[8];
		Vector3<> points[8];

		void Initialize(
			const float blf, const float tlf,
			const float blb, const float tlb,
			const float brf, const float trf,
			const float brb, const float trb
		) {
			values[0] = blf; values[1] = brf; values[2] = brb; values[3] = blb;
			values[4] = tlf; values[5] = trf; values[6] = trb; values[7] = tlb;
			points[0] = Vector3<>(0, 0, 0);
			points[1] = Vector3<>(1, 0, 0);
			points[2] = Vector3<>(1, 1, 0);
			points[3] = Vector3<>(0, 1, 0);
			points[4] = Vector3<>(0, 0, 1);
			points[5] = Vector3<>(1, 0, 1);
			points[6] = Vector3<>(1, 1, 1);
			points[7] = Vector3<>(0, 1, 1);
		}
	};

	template<typename T>
	struct Tensor2 {
		std::vector<T> Data;
		uint32 Width;        // X
		uint32 Depth;        // Y

		Tensor2() : Tensor2(0, 0) {}
		Tensor2(const uint32 size) : Tensor2(size, size) {}
		Tensor2(const uint32 size, const T & value) : Tensor2(size, size, value) {}

		Tensor2(const Vector2<uint16> & size) : Tensor2(size.X, size.Y) {}
		Tensor2(const Vector2<uint16> & size, const T & value) :
			Tensor2(size.X, size.Y, value) {}
		Tensor2(const Vector2<uint32> & size) : Tensor2(size.X, size.Y) {}
		Tensor2(const Vector2<uint32> & size, const T & value) :
			Tensor2(size.X, size.Y, value) {}

		Tensor2(uint32 width, uint32 depth) :
			Width(width), Depth(depth), Data(width * depth) {}
		Tensor2(uint32 width, uint32 depth, const T & value) :
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
		uint32 Width;       // X
		uint32 Depth;       // Y
		uint32 Height;      // Z

		Tensor3() : Tensor3(0, 0, 0) {}
		Tensor3(const uint32 size) : Tensor3(size, size, size) {}
		Tensor3(const uint32 size, const T & value) : Tensor3(size, size, size, value) {}
		Tensor3(const Vector3<uint16> & size) : Tensor3(size.X, size.Y, size.Z) {}
		Tensor3(const Vector3<uint16> & size, const T & value) :
			Tensor3(size.X, size.Y, size.Z, value) {}
		Tensor3(const Vector3<uint32> & size) : Tensor3(size.X, size.Y, size.Z) {}
		Tensor3(const Vector3<uint32> & size, const T & value) :
			Tensor3(size.X, size.Y, size.Z, value) {}

		Tensor3(uint32 width, uint32 depth, uint32 height) :
			Width(width), Height(height), Depth(depth), Data(width * height * depth) {}
		Tensor3(uint32 width, uint32 depth, uint32 height, const T & value) :
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

#include "Algebra2.h"
#include "Algebra3.h"
