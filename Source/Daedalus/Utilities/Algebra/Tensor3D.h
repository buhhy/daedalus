#pragma once

#include <Utilities/Algebra/Vector3D.h>

#include <cassert>
#include <vector>
#include <sstream>

namespace utils {
	template<typename T>
	class Tensor3DBase {
	protected:
		std::vector<T> Data;
		Uint32 Width;       // X
		Uint32 Depth;       // Y
		Uint32 Height;      // Z

		inline void CheckBounded(const Uint32 w, const Uint32 d, const Uint32 h) const {
			if (w < 0 && w >= Width) {
				std::stringstream ss;
				ss << "Tensor3DBase::CheckBounded: Width `" << w <<
					"` is out of bounds [0, " << Width << ").";
				throw StringException(ss.str());
			}
			if (h < 0 && h >= Height) {
				std::stringstream ss;
				ss << "Tensor3DBase::CheckBounded: Height `" << h <<
					"` is out of bounds [0, " << Height << ").";
				throw StringException(ss.str());
			}
			if (d < 0 && d >= Depth) {
				std::stringstream ss;
				ss << "Tensor3DBase::CheckBounded: Depth `" << d <<
					"` is out of bounds [0, " << Depth << ").";
				throw StringException(ss.str());
			}
		}

	public:
		explicit Tensor3DBase() : Tensor3DBase(0, 0, 0) {}
		explicit Tensor3DBase(const Uint32 size) : Tensor3DBase<T>(size, size, size) {}
		explicit Tensor3DBase(const Vector3D<Uint32> & size) :
			Tensor3DBase(size.X, size.Y, size.Z)
		{}
		Tensor3DBase(const Vector3D<Uint32> & size, const T & value) :
			Tensor3DBase(size.X, size.Y, size.Z, value)
		{}
		Tensor3DBase(const Uint32 width, const Uint32 depth, const Uint32 height) :
			Width(width), Height(height), Depth(depth), Data(width * height * depth)
		{}
		Tensor3DBase(const Uint32 width, const Uint32 depth, const Uint32 height, const T & value) :
			Width(width), Height(height), Depth(depth), Data(width * height * depth, value)
		{}

		Uint32 GetWidth() const { return Width; }
		Uint32 GetDepth() const { return Depth; }
		Uint32 GetHeight() const { return Height; }
		Vector3D<Uint32> Size() const { return Vector3D<Uint32>(Width, Depth, Height); }
	};



	template <typename T>
	class Tensor3D : public Tensor3DBase<T> {
	public:
		explicit Tensor3D() : Tensor3DBase<T>(0, 0, 0) {}
		explicit Tensor3D(const Uint32 size) : Tensor3DBase<T>(size, size, size) {}
		explicit Tensor3D(const Vector3D<Uint32> & size) :
			Tensor3DBase<T>(size.X, size.Y, size.Z)
		{}
		Tensor3D(const Vector3D<Uint32> & size, const T & value) :
			Tensor3DBase<T>(size.X, size.Y, size.Z, value)
		{}
		Tensor3D(const Uint32 width, const Uint32 depth, const Uint32 height) :
			Tensor3DBase<T>(width, height, depth)
		{}
		Tensor3D(const Uint32 width, const Uint32 depth, const Uint32 height, const T & value) :
			Tensor3DBase<T>(width, height, depth, value)
		{}

		const T & Get(const Uint32 & x, const Uint32 & y, const Uint32 & z) const {
			this->CheckBounded(x, y, z);
			return this->Data[(x * this->Width + y) * this->Height + z];
		}

		T & Get(const Uint32 & x, const Uint32 & y, const Uint32 & z) {
			this->CheckBounded(x, y, z);
			return this->Data[(x * this->Width + y) * this->Height + z];
		}

		const T & Get(const Vector3D<Uint32> & vec) const { return Get(vec.X, vec.Y, vec.Z); }
		T & Get(const Vector3D<Uint32> & vec) { return Get(vec.X, vec.Y, vec.Z); }

		void Set(const Uint32 & x, const Uint32 & y, const Uint32 & z, const T & value) {
			this->CheckBounded(x, y, z);
			this->Data[(x * this->Width + y) * this->Height + z] = value;
		}

		void Fill(const T & value) {
			this->Data.assign(this->Width * this->Height * this->Depth, value);
		}
	};


	
	// Specialization for Tensor3D<bool> because vector<bool> has a special implementation
	template <>
	class Tensor3D<bool> : public Tensor3DBase<bool> {
	public:
		Tensor3D() : Tensor3DBase(0, 0, 0) {}
		Tensor3D(const Vector3D<Uint32> & size) : Tensor3DBase(size.X, size.Y, size.Z) {}
		Tensor3D(const Vector3D<Uint32> & size, const bool value) :
			Tensor3DBase(size.X, size.Y, size.Z, value)
		{}
		Tensor3D(const Uint32 width, const Uint32 depth, const Uint32 height) :
			Tensor3DBase(width, height, depth)
		{}
		Tensor3D(const Uint32 width, const Uint32 depth, const Uint32 height, const bool value) :
			Tensor3DBase(width, height, depth, value)
		{}

		bool Get(const Uint32 & x, const Uint32 & y, const Uint32 & z) const {
			CheckBounded(x, y, z);
			return this->Data[(x * this->Width + y) * this->Height + z];
		}

		bool Get(const Vector3D<Uint32> & vec) const { return Get(vec.X, vec.Y, vec.Z); }

		void Set(const Uint32 & x, const Uint32 & y, const Uint32 & z, const bool value) {
			CheckBounded(x, y, z);
			this->Data[(x * this->Width + y) * this->Height + z] = value;
		}

		void Fill(const bool value) {
			this->Data.assign(this->Width * this->Height * this->Depth, value);
		}
	};



	template <typename T>
	class TensorResizable3D : public Tensor3D<T> {
	public:
		TensorResizable3D() : Tensor3D<T>(0, 0, 0) {}
		TensorResizable3D(const Vector3D<Uint32> & size) : Tensor3D<T>(size.X, size.Y, size.Z) {}
		TensorResizable3D(const Vector3D<Uint32> & size, const T & value) :
			Tensor3D<T>(size.X, size.Y, size.Z, value)
		{}
		TensorResizable3D(const Uint32 width, const Uint32 depth, const Uint32 height) :
			Tensor3D<T>(width, height, depth)
		{}
		TensorResizable3D(
			const Uint32 width, const Uint32 depth,
			const Uint32 height, const T & value
		) : Tensor3D<T>(width, height, depth, value)
		{}

		TensorResizable3D & Reset(
			const Uint32 Width, const Uint32 Height,
			const Uint32 Depth, const T & value
		) {
			this->Width = Width;
			this->Height = Height;
			this->Depth = Depth;
			this->Data.resize(Width * Height * Depth, value);

			return *this;
		}
	};


	
	template <typename T, Uint32 S>
	class TensorFixed3D : public Tensor3D<T> {
	public:
		TensorFixed3D() : Tensor3D<T>(S) {}
		TensorFixed3D(const T & value) : Tensor3D<T>(S, S, S, value) {}
	};
}
