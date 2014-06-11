#pragma once

#include "Vector3.h"

namespace utils {
	template <typename T = double>
	struct Matrix4 {
	private:
		T Values[16];
	
	public:
		Matrix4() {}

		Matrix4(
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

		Matrix4<T> & operator = (const Matrix4<T> & other) {
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

	Matrix4<> CreateRotation(const double angle,  const Axis axis);
	Matrix4<> CreateRotation(const Vector3<> & x,  const Vector3<> & y,  const Vector3<> & z);
	Matrix4<> CreateTranslation(const Vector3<> & displacement);
	Matrix4<> CreateScaling(const Vector3<> & scale);
}
