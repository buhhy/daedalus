#pragma once

#include <Utilities/Algebra/Vector3D.h>

namespace utils {
	template <typename T = double>
	struct Matrix4D {
	private:
		T Values[16];

		void swaprows(size_t r1, size_t r2) {
			std::swap((*this)[r1][0], (*this)[r2][0]);
			std::swap((*this)[r1][1], (*this)[r2][1]);
			std::swap((*this)[r1][2], (*this)[r2][2]);
			std::swap((*this)[r1][3], (*this)[r2][3]);
		}

		void dividerow(size_t r, double fac) {
			(*this)[r][0] /= fac;
			(*this)[r][1] /= fac;
			(*this)[r][2] /= fac;
			(*this)[r][3] /= fac;
		}

		void submultrow(size_t dest, size_t src, double fac) {
			(*this)[dest][0] -= fac * (*this)[src][0];
			(*this)[dest][1] -= fac * (*this)[src][1];
			(*this)[dest][2] -= fac * (*this)[src][2];
			(*this)[dest][3] -= fac * (*this)[src][3];
		}

		inline void Set(const Uint32 column, const Uint32 row, const T value) {
			Values[row * 4 + column] = value;
		}
	
	public:
		Matrix4D() {}

		Matrix4D(
			const T & c00, const T & c01, const T & c02, const T & c03,
			const T & c10, const T & c11, const T & c12, const T & c13,
			const T & c20, const T & c21, const T & c22, const T & c23,
			const T & c30, const T & c31, const T & c32, const T & c33
		) {
			Values[0] = c00; Values[1] = c01; Values[2] = c02; 	Values[3] = c03; 
			Values[4] = c10; Values[5] = c11; Values[6] = c12; Values[7] = c13; 
			Values[8] = c20; Values[9] = c21; Values[10] = c22; Values[11] = c23; 
			Values[12] = c30; Values[13] = c31; Values[14] = c32; Values[15] = c33; 
		}

		const T * operator [] (size_t row) const { return Values + row * 4; }
		T * operator [] (size_t row) { return Values + row * 4; }

		const T & Get(const Uint32 column, const Uint32 row) const {
			return Values[row * 4 + column];
		}
		T & Get(const Uint32 column, const Uint32 row) {
			return Values[row * 4 + column];
		}

		Matrix4D<T> & operator = (const Matrix4D<T> & other) {
			// Assignment swap principle
			T Temp[16];
			for (size_t i = 0; i < 16; i++) Temp[i] = other.Values[i];
			for (size_t i = 0; i < 16; i++) Values[i] = Temp[i];
			return *this;
		}

		Matrix4D Transpose() const {
			return Matrix4D<T>(
				Values[0], Values[4], Values[8], Values[12],
				Values[1], Values[5], Values[9], Values[13],
				Values[2], Values[6], Values[10], Values[14],
				Values[3], Values[7], Values[11], Values[15]);
		}

		Matrix4D Invert() const {
			// The algorithm is plain old Gauss-Jordan elimination with partial pivoting.

			Matrix4D a(*this);
			Matrix4D ret;

			// Loop over cols of a from left to right, eliminating above and below diag.

			/* Find largest pivot in column j among rows j..3 */
			for (size_t j = 0; j < 4; ++j) { 
				size_t i1 = j; /* Row with largest pivot candidate */
				for (size_t i = j + 1; i < 4; ++i) {
					if (fabs(a.Get(i, j)) > fabs(a.Get(i1, j)))
						i1 = i;
				}

				/* Swap rows i1 and j in a and ret to put pivot on diagonal */
				a.swaprows(a, i1, j);
				ret.swaprows(ret, i1, j);

				/* Scale row j to have a unit diagonal */
				if (a.Get(j, j) == 0.0) {
					// Theoretically throw an exception.
					return ret;
				}

				ret.dividerow(j, a.Get(j, j));
				a.dividerow(j, a(j, j));

				/* Eliminate off-diagonal elems in col j of a, doing identical ops to b */
				for (size_t i = 0; i < 4; ++i) {
					if(i != j) {
						ret.submultrow(ret, i, j, a.Get(i, j));
						a.submultrow(a, i, j, a(i, j));
					}
				}
			}

			return ret;
		}
	};

	enum Axis {
		AXIS_X,
		AXIS_Y,
		AXIS_Z
	};
	
	/**
	 * Return a matrix to represent a counterclockwise rotation of "angle"
	 * degrees around the axis "axis", where "axis" is one of the
	 * characters 'x', 'y', or 'z'.
	 */
	Matrix4D<> CreateRotation(const double angle,  const Axis axis);

	/**
	 * Returns a rotation matrix built around the provided basis XYZ vectors.
	 */
	Matrix4D<> CreateRotation(const Vector3D<> & x, const Vector3D<> & y, const Vector3D<> & z);

	// Return a matrix to represent a displacement of the given vector.
	Matrix4D<> CreateTranslation(const Vector3D<> & displacement);

	// Return a matrix to represent a nonuniform scale with the given factors.
	Matrix4D<> CreateScaling(const Vector3D<> & scale);

	struct Basis3D;
		
	Basis3D GetBasisFrom(const Matrix4D<double> & mat);
	Vector3D<> GetTranslationVectorFrom(const Matrix4D<double> & mat);
}
