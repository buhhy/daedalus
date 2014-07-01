#pragma once

#include <Utilities/Algebra/Vector2.h>
#include <Utilities/Algebra/Vector3.h>
#include <Utilities/Algebra/Vector4.h>
#include <Utilities/Algebra/Matrix4.h>

#include <cstdint>
#include <vector>
#include <memory>

using std::int8_t;
using std::int16_t;
using std::int32_t;
using std::int64_t;

using std::uint8_t;
using std::uint16_t;
using std::uint32_t;
using std::uint64_t;

namespace utils {
	// Equivalent to the Scala Option construct which can be a Some or None value
	template <typename T>
	struct Option {
	private:
		std::shared_ptr<T> Value;
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
		double Radius;

		Circle2D(const Vector2<> center, const double radius) : Center(center), Radius(radius) {}
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
		uint32_t Width;        // X
		uint32_t Depth;        // Y

		Tensor2() : Tensor2(0, 0) {}
		Tensor2(const uint32_t size) : Tensor2(size, size) {}
		Tensor2(const uint32_t size, const T & value) : Tensor2(size, size, value) {}

		Tensor2(const Vector2<uint16_t> & size) : Tensor2(size.X, size.Y) {}
		Tensor2(const Vector2<uint16_t> & size, const T & value) :
			Tensor2(size.X, size.Y, value) {}
		Tensor2(const Vector2<uint32_t> & size) : Tensor2(size.X, size.Y) {}
		Tensor2(const Vector2<uint32_t> & size, const T & value) :
			Tensor2(size.X, size.Y, value) {}

		Tensor2(const uint32_t width, const uint32_t depth) :
			Width(width), Depth(depth), Data(width * depth) {}
		Tensor2(const uint32_t width, const uint32_t depth, const T & value) :
			Width(width), Depth(depth), Data(width * depth, value) {}

		Tensor2 & Reset(uint32_t Width, uint32_t Depth, const T & value) {
			this->Width = Width;
			this->Depth = Depth;
			Data.resize(Width * Depth, value);

			return *this;
		}

		const T & Get(const uint32_t x, const uint32_t y) const {
			const T & temp = Data.at(x * Width + y); return temp;   // WTF?
		}

		T & Get(const uint32_t x, const uint32_t y) {
			return Data.at(x * Width + y);   // WTF?
		}

		void Set(const uint32_t x, const uint32_t y, const T & value) {
			Data[x * Width + y] = value;
		}

		void Fill(const T & value) {
			Data.assign(Width * Depth, value);
		}
	};

	// Specialization for Tensor2<bool> because vector<bool> has a special implementation
	template<>
	struct Tensor2<bool> {
		std::vector<bool> Data;
		uint32_t Width;        // X
		uint32_t Depth;        // Y

		Tensor2() : Tensor2(0u, 0u) {}
		Tensor2(const uint32_t size) : Tensor2(size, size) {}

		Tensor2(const Vector2<uint16_t> & size) : Tensor2(size.X, size.Y) {}
		Tensor2(const Vector2<uint16_t> & size, const bool value) :
			Tensor2(size.X, size.Y, value) {}
		Tensor2(const Vector2<uint32_t> & size) : Tensor2(size.X, size.Y) {}
		Tensor2(const Vector2<uint32_t> & size, const bool value) :
			Tensor2(size.X, size.Y, value) {}

		Tensor2(const uint32_t width, const uint32_t depth) :
			Width(width), Depth(depth), Data(width * depth) {}
		Tensor2(const uint32_t width, const uint32_t depth, const bool value) :
			Width(width), Depth(depth), Data(width * depth, value) {}

		Tensor2 & Reset(const uint32_t Width, const uint32_t Depth, const bool value) {
			this->Width = Width;
			this->Depth = Depth;
			Data.resize(Width * Depth, value);

			return *this;
		}

		bool Get(const uint32_t x, const uint32_t y) const { return Data.at(x * Width + y); }
		bool Get(const uint32_t x, const uint32_t y) { return Data.at(x * Width + y); }

		void Set(const uint32_t x, const uint32_t y, const bool value) {
			Data[x * Width + y] = value;
		}

		void Fill(const bool value) { Data.assign(Width * Depth, value); }
	};

	template<typename T>
	struct Tensor3 {
		std::vector<T> Data;
		uint32_t Width;       // X
		uint32_t Depth;       // Y
		uint32_t Height;      // Z

		Tensor3() : Tensor3(0, 0, 0) {}
		Tensor3(const uint32_t size) : Tensor3(size, size, size) {}
		Tensor3(const uint32_t size, const T & value) : Tensor3(size, size, size, value) {}
		Tensor3(const Vector3<uint16_t> & size) : Tensor3(size.X, size.Y, size.Z) {}
		Tensor3(const Vector3<uint16_t> & size, const T & value) :
			Tensor3(size.X, size.Y, size.Z, value) {}
		Tensor3(const Vector3<uint32_t> & size) : Tensor3(size.X, size.Y, size.Z) {}
		Tensor3(const Vector3<uint32_t> & size, const T & value) :
			Tensor3(size.X, size.Y, size.Z, value) {}

		Tensor3(uint32_t width, uint32_t depth, uint32_t height) :
			Width(width), Height(height), Depth(depth), Data(width * height * depth) {}
		Tensor3(uint32_t width, uint32_t depth, uint32_t height, const T & value) :
			Width(width), Height(height), Depth(depth), Data(width * height * depth, value) {}

		Tensor3 & Reset(uint32_t Width, uint32_t Height, uint32_t Depth, const T & value) {
			this->Width = Width;
			this->Height = Height;
			this->Depth = Depth;
			Data.resize(Width * Height * Depth, value);

			return *this;
		}

		const T & Get(const uint32_t & x, const uint32_t & y, const uint32_t & z) const {
			return Data[(x * Width + y) * Height + z];
		}

		T & GetMutable(const uint32_t & x, const uint32_t & y, const uint32_t & z) {
			return Data[(x * Width + y) * Height + z];
		}

		void Set(const uint32_t & x, const uint32_t & y, const uint32_t & z, const T & value) {
			Data[(x * Width + y) * Height + z] = value;
		}

		void Fill(const T & value) {
			Data.assign(Width * Height * Depth, value);
		}
	};
}
