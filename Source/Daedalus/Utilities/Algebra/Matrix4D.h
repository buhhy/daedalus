#pragma once

#include "Vector3D.h"

namespace utils {
	template <typename T = double>
	struct Matrix4D {
	private:
		T Values[16];
	
	public:
		Matrix4D() {}

		Matrix4D(
			const T & c00, const T & c01, const T & c02, const T & c03,
			const T & c10, const T & c11, const T & c12, const T & c13,
			const T & c20, const T & c21, const T & c22, const T & c23,
			const T & c30, const T & c31, const T & c32, const T & c33
		) {
			Values[0] = c00; 
			Values[1] = c01; 
			Values[2] = c02; 
			Values[3] = c03; 

			Values[4] = c10; 
			Values[5] = c11; 
			Values[6] = c12; 
			Values[7] = c13; 

			Values[8] = c20; 
			Values[9] = c21; 
			Values[10] = c22; 
			Values[11] = c23; 

			Values[12] = c30; 
			Values[13] = c31; 
			Values[14] = c32; 
			Values[15] = c33; 
		}

		const T * operator [] (size_t row) const { return Values + row * 4; }
		T * operator [] (size_t row) { return Values + row * 4; }

		const T & Get(const Uint32 column, const Uint32 row) const {
			return Values[row * 4 + column];
		}
		T & Get(const Uint32 column, const Uint32 row) {
			return Values[row * 4 + column];
		}
		
		void GetBasis(Vector3D<T> & x, Vector3D<T> & y, Vector3D<T> & z) const {
			x.Reset(Get(0, 0), Get(0, 1), Get(0, 2));
			y.Reset(Get(1, 0), Get(1, 1), Get(1, 2));
			z.Reset(Get(2, 0), Get(2, 1), Get(2, 2));
		}

		Vector3D<T> GetTranslationVector() const {
			return { Get(3, 0), Get(3, 1), Get(3, 2) };
		}

		Matrix4D<T> & operator = (const Matrix4D<T> & other) {
			// Assignment swap principle
			T Temp[16];
			for (size_t i = 0; i < 16; i++) Temp[i] = other.Values[i];
			for (size_t i = 0; i < 16; i++) Values[i] = Temp[i];
			return *this;
		}
	};

	enum Axis {
		AXIS_X,
		AXIS_Y,
		AXIS_Z
	};

	Matrix4D<> CreateRotation(const double angle,  const Axis axis);
	Matrix4D<> CreateRotation(const Vector3D<> & x, const Vector3D<> & y, const Vector3D<> & z);
	Matrix4D<> CreateTranslation(const Vector3D<> & displacement);
	Matrix4D<> CreateScaling(const Vector3D<> & scale);
}
