#pragma once

#include <Utilities/Integers.h>
#include <Utilities/Algebra/Vector3D.h>
#include <Utilities/Algebra/Vector4D.h>
#include <Utilities/Algebra/Matrix4D.h>

#include <vector>
#include <memory>

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

	// TODO: make actual colour class
	using Colour = Vector3D<Uint8>;

	struct Triangle3D {
		Vector3D<> Point1, Point2, Point3;

		Triangle3D(
			const Vector3D<> & p1,
			const Vector3D<> & p2,
			const Vector3D<> & p3
		) : Point1(p1), Point2(p2), Point3(p3) {}
	};

	// TODO: get rid of this
	struct GridCell {
		float values[8];
		Vector3D<> points[8];

		void Initialize(
			const float blf, const float tlf,
			const float blb, const float tlb,
			const float brf, const float trf,
			const float brb, const float trb
		) {
			values[0] = blf; values[1] = brf; values[2] = brb; values[3] = blb;
			values[4] = tlf; values[5] = trf; values[6] = trb; values[7] = tlb;
			points[0] = Vector3D<>(0, 0, 0);
			points[1] = Vector3D<>(1, 0, 0);
			points[2] = Vector3D<>(1, 1, 0);
			points[3] = Vector3D<>(0, 1, 0);
			points[4] = Vector3D<>(0, 0, 1);
			points[5] = Vector3D<>(1, 0, 1);
			points[6] = Vector3D<>(1, 1, 1);
			points[7] = Vector3D<>(0, 1, 1);
		}
	};

	template<typename T>
	struct Tensor3 {
		std::vector<T> Data;
		size_t Width;       // X
		size_t Depth;       // Y
		size_t Height;      // Z

		Tensor3() : Tensor3(0, 0, 0) {}
		Tensor3(const size_t size) : Tensor3(size, size, size) {}
		Tensor3(const size_t size, const T & value) : Tensor3(size, size, size, value) {}
		Tensor3(const Vector3D<Uint16> & size) : Tensor3(size.X, size.Y, size.Z) {}
		Tensor3(const Vector3D<Uint16> & size, const T & value) :
			Tensor3(size.X, size.Y, size.Z, value) {}
		Tensor3(const Vector3D<size_t> & size) : Tensor3(size.X, size.Y, size.Z) {}
		Tensor3(const Vector3D<size_t> & size, const T & value) :
			Tensor3(size.X, size.Y, size.Z, value) {}

		Tensor3(size_t width, size_t depth, size_t height) :
			Width(width), Height(height), Depth(depth), Data(width * height * depth) {}
		Tensor3(size_t width, size_t depth, size_t height, const T & value) :
			Width(width), Height(height), Depth(depth), Data(width * height * depth, value) {}

		Tensor3 & Reset(size_t Width, size_t Height, size_t Depth, const T & value) {
			this->Width = Width;
			this->Height = Height;
			this->Depth = Depth;
			Data.resize(Width * Height * Depth, value);

			return *this;
		}

		const T & Get(const size_t & x, const size_t & y, const size_t & z) const {
			return Data[(x * Width + y) * Height + z];
		}

		T & GetMutable(const size_t & x, const size_t & y, const size_t & z) {
			return Data[(x * Width + y) * Height + z];
		}

		void Set(const size_t & x, const size_t & y, const size_t & z, const T & value) {
			Data[(x * Width + y) * Height + z] = value;
		}

		void Fill(const T & value) {
			Data.assign(Width * Height * Depth, value);
		}
	};
}
