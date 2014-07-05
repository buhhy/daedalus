#pragma once

#include <Utilities/Algebra/Vector2.h>

#include <vector>

namespace utils {
	template <typename T>
	class Tensor2DBase {
	protected:
		std::vector<T> Data;
		size_t Width;        // X
		size_t Depth;        // Y

	public:
		Tensor2DBase() :Tensor2DBase(0, 0) {}
		Tensor2DBase(const Vector2<size_t> & size) : Tensor2DBase(size.X, size.Y) {}
		Tensor2DBase(const Vector2<size_t> & size, const T & value) :
			Tensor2DBase(size.X, size.Y, value)
		{}
		Tensor2DBase(const size_t width, const size_t depth) :
			Width(width), Depth(depth), Data(width * depth)
		{}
		Tensor2DBase(const size_t width, const size_t depth, const T & value) :
			Width(width), Depth(depth), Data(width * depth, value)
		{}

		size_t GetWidth() const { return Width; }
		size_t GetDepth() const { return Depth; }
	};

	template <typename T>
	class Tensor2D : public Tensor2DBase<T> {
	private:
		using Tensor2DBase<T>::Data;
		using Tensor2DBase<T>::Width;
		using Tensor2DBase<T>::Depth;

	public:
		Tensor2D() : Tensor2DBase<T>(0, 0) {}
		Tensor2D(const Vector2<size_t> & size) : Tensor2DBase<T>(size.X, size.Y) {}
		Tensor2D(const Vector2<size_t> & size, const T & value) :
			Tensor2DBase<T>(size.X, size.Y, value)
		{}
		Tensor2D(const size_t width, const size_t depth) :
			Tensor2DBase<T>(width, depth)
		{}
		Tensor2D(const size_t width, const size_t depth, const T & value) :
			Tensor2DBase<T>(width, depth, value)
		{}

		const T & Get(const size_t x, const size_t y) const {
			const T & temp = Data.at(x * Width + y); return temp;
		}

		T & Get(const size_t x, const size_t y) {
			return Data.at(x * Width + y);
		}

		void Set(const size_t x, const size_t y, const T & value) {
			Data[x * Width + y] = value;
		}

		void Fill(const T & value) {
			Data.assign(Width * Depth, value);
		}
	};



	// Specialization for Tensor2D<bool> because vector<bool> has a special implementation
	template <>
	class Tensor2D<bool> : public Tensor2DBase<bool> {
	private:
		using Tensor2DBase<bool>::Data;
		using Tensor2DBase<bool>::Width;
		using Tensor2DBase<bool>::Depth;

	public:
		Tensor2D() :Tensor2DBase(0, 0) {}
		Tensor2D(const Vector2<size_t> & size) : Tensor2DBase(size.X, size.Y) {}
		Tensor2D(const Vector2<size_t> & size, const bool value) :
			Tensor2DBase(size.X, size.Y, value)
		{}
		Tensor2D(const size_t width, const size_t depth) :
			Tensor2DBase(width, depth)
		{}
		Tensor2D(const size_t width, const size_t depth, const bool value) :
			Tensor2DBase(width, depth, value)
		{}

		bool Get(const size_t x, const size_t y) const { return Data.at(x * Width + y); }
		bool Get(const size_t x, const size_t y) { return Data.at(x * Width + y); }

		void Set(const size_t x, const size_t y, const bool value) {
			Data[x * Width + y] = value;
		}

		void Fill(const bool value) {
			Data.assign(Width * Depth, value);
		}
	};



	template <typename T>
	class TensorResizable2D : public Tensor2D<T> {
	private:
		using Tensor2D<T>::Data;
		using Tensor2D<T>::Width;
		using Tensor2D<T>::Depth;

	public:
		TensorResizable2D() :Tensor2D<T>(0, 0) {}
		TensorResizable2D(const Vector2<size_t> & size) : Tensor2D<T>(size.X, size.Y) {}
		TensorResizable2D(const Vector2<size_t> & size, const T & value) :
			Tensor2D<T>(size.X, size.Y, value)
		{}
		TensorResizable2D(const size_t width, const size_t depth) :
			Tensor2D<T>(width, depth)
		{}
		TensorResizable2D(const size_t width, const size_t depth, const T & value) :
			Tensor2D<T>(width, depth, value)
		{}

		TensorResizable2D & Reset(size_t Width, size_t Depth, const T & value) {
			this->Width = Width;
			this->Depth = Depth;
			Data.resize(Width * Depth, value);

			return *this;
		}
	};


	
	template <typename T, size_t S>
	class TensorFixed2D : public Tensor2D<T> {
	public:
		TensorFixed2D() : Tensor2D<T>(S) {}
		TensorFixed2D(const T & value) : Tensor2D<T>(S, S, value) {}
	};
}
