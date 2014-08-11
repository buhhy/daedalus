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
		size_t Width;       // X
		size_t Depth;       // Y
		size_t Height;      // Z

		inline void CheckBounded(const size_t w, const size_t d, const size_t h) const {
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
		Tensor3DBase() : Tensor3DBase(0, 0, 0) {}
		Tensor3DBase(const Vector3D<size_t> & size) :
			Tensor3DBase(size.X, size.Y, size.Z)
		{}
		Tensor3DBase(const Vector3D<size_t> & size, const T & value) :
			Tensor3DBase(size.X, size.Y, size.Z, value)
		{}
		Tensor3DBase(const size_t width, const size_t depth, const size_t height) :
			Width(width), Height(height), Depth(depth), Data(width * height * depth)
		{}
		Tensor3DBase(const size_t width, const size_t depth, const size_t height, const T & value) :
			Width(width), Height(height), Depth(depth), Data(width * height * depth, value)
		{}

		size_t GetWidth() const { return Width; }
		size_t GetDepth() const { return Depth; }
		size_t GetHeight() const { return Height; }
	};



	template <typename T>
	class Tensor3D : public Tensor3DBase<T> {
	public:
		Tensor3D() : Tensor3DBase<T>(0, 0, 0) {}
		Tensor3D(const Vector3D<size_t> & size) : Tensor3DBase<T>(size.X, size.Y, size.Z) {}
		Tensor3D(const Vector3D<size_t> & size, const T & value) :
			Tensor3DBase<T>(size.X, size.Y, size.Z, value)
		{}
		Tensor3D(const size_t width, const size_t depth, const size_t height) :
			Tensor3DBase<T>(width, height, depth)
		{}
		Tensor3D(const size_t width, const size_t depth, const size_t height, const T & value) :
			Tensor3DBase<T>(width, height, depth, value)
		{}

		const T & Get(const size_t & x, const size_t & y, const size_t & z) const {
			this->CheckBounded(x, y, z);
			return this->Data[(x * this->Width + y) * this->Height + z];
		}

		T & Get(const size_t & x, const size_t & y, const size_t & z) {
			this->CheckBounded(x, y, z);
			return this->Data[(x * this->Width + y) * this->Height + z];
		}

		void Set(const size_t & x, const size_t & y, const size_t & z, const T & value) {
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
		Tensor3D(const Vector3D<size_t> & size) : Tensor3DBase(size.X, size.Y, size.Z) {}
		Tensor3D(const Vector3D<size_t> & size, const bool value) :
			Tensor3DBase(size.X, size.Y, size.Z, value)
		{}
		Tensor3D(const size_t width, const size_t depth, const size_t height) :
			Tensor3DBase(width, height, depth)
		{}
		Tensor3D(const size_t width, const size_t depth, const size_t height, const bool value) :
			Tensor3DBase(width, height, depth, value)
		{}

		bool Get(const size_t & x, const size_t & y, const size_t & z) const {
			CheckBounded(x, y, z);
			return this->Data[(x * this->Width + y) * this->Height + z];
		}

		void Set(const size_t & x, const size_t & y, const size_t & z, const bool value) {
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
		TensorResizable3D(const Vector3D<size_t> & size) : Tensor3D<T>(size.X, size.Y, size.Z) {}
		TensorResizable3D(const Vector3D<size_t> & size, const T & value) :
			Tensor3D<T>(size.X, size.Y, size.Z, value)
		{}
		TensorResizable3D(const size_t width, const size_t depth, const size_t height) :
			Tensor3D<T>(width, height, depth)
		{}
		TensorResizable3D(
			const size_t width, const size_t depth,
			const size_t height, const T & value
		) : Tensor3D<T>(width, height, depth, value)
		{}

		TensorResizable3D & Reset(
			const size_t Width, const size_t Height,
			const size_t Depth, const T & value
		) {
			this->Width = Width;
			this->Height = Height;
			this->Depth = Depth;
			this->Data.resize(Width * Height * Depth, value);

			return *this;
		}
	};


	
	template <typename T, size_t S>
	class TensorFixed3D : public Tensor3D<T> {
	public:
		TensorFixed3D() : Tensor3D<T>(S) {}
		TensorFixed3D(const T & value) : Tensor3D<T>(S, S, S, value) {}
	};
}
