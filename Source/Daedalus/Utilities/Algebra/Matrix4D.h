#pragma once

#include <Utilities/Algebra/Vector3D.h>

namespace utils {
	template <typename T = double>
	struct Matrix4D {
	private:
		T Values[16];

		void SwapRows(Uint32 r1, Uint32 r2) {
			for (Uint8 i = 0; i < 4; i++) {
				const T val = Get(i, r2);
				Set(i, r2, Get(i, r1));
				Set(i, r1, val);
			}
		}

		void DivideRow(Uint32 r, double fac) {
			for (Uint8 i = 0; i < 4; i++)
				Get(i, r) /= fac;
		}

		void SubtractMultiplyRow(Uint32 dest, Uint32 src, double fac) {
			for (Uint8 i = 0; i < 4; i++)
				Get(i, dest) -= fac * Get(i, src);
		}
	
	public:
		static Matrix4D<> IdentityMatrix() {
			Matrix4D<> ret(0);
			for (Uint8 i = 0; i < 4; i++)
				ret.Set(i, i, 1);
			return ret;
		}

		Matrix4D() {}
		
		explicit Matrix4D(const T & fill) {
			for (Uint8 i = 0; i < 16; i++)
				Values[i] = fill;
		}

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

		const T * operator [] (Uint32 row) const { return Values + row * 4; }
		T * operator [] (Uint32 row) { return Values + row * 4; }

		const T & Get(const Uint32 column, const Uint32 row) const {
			return Values[row * 4 + column];
		}
		T & Get(const Uint32 column, const Uint32 row) {
			return Values[row * 4 + column];
		}

		Vector4D<T> GetRow(Uint32 row) const {
			return Vector4D<T>(Get(0, row), Get(1, row), Get(2, row), Get(3, row));
		}

		Vector4D<T> GetColumn(Uint32 column) const {
			return Vector4D<T>(Get(column, 0), Get(column, 1), Get(column, 2), Get(column, 3));
		}

		void Set(const Uint32 column, const Uint32 row, const T value) {
			Values[row * 4 + column] = value;
		}

		Matrix4D<T> & operator = (const Matrix4D<T> & other) {
			// Assignment swap principle
			T Temp[16];
			for (size_t i = 0; i < 16; i++) Temp[i] = other.Values[i];
			for (size_t i = 0; i < 16; i++) Values[i] = Temp[i];
			return *this;
		}

		Matrix4D<T> Transpose() const {
			return Matrix4D<T>(
				Values[0], Values[4], Values[8], Values[12],
				Values[1], Values[5], Values[9], Values[13],
				Values[2], Values[6], Values[10], Values[14],
				Values[3], Values[7], Values[11], Values[15]);
		}

		Matrix4D<T> Invert() const {
			// The algorithm is plain old Gauss-Jordan elimination with partial pivoting.

			Matrix4D<T> a(*this);
			Matrix4D<T> ret = IdentityMatrix();

			// Loop over cols of a from left to right, eliminating above and below diag.

			/* Find largest pivot in column j among rows j..3 */
			for (Uint32 j = 0; j < 4; ++j) { 
				Uint32 i1 = j; /* Row with largest pivot candidate */
				for (Uint32 i = j + 1; i < 4; ++i) {
					if (fabs(a.Get(j, i)) > fabs(a.Get(j, i1)))
						i1 = i;
				}

				/* Swap rows i1 and j in a and ret to put pivot on diagonal */
				a.SwapRows(i1, j);
				ret.SwapRows(i1, j);

				/* Scale row j to have a unit diagonal */
				if (a.Get(j, j) == 0.0) {
					// Theoretically throw an exception.
					return ret;
				}

				ret.DivideRow(j, a.Get(j, j));
				a.DivideRow(j, a.Get(j, j));

				/* Eliminate off-diagonal elems in col j of a, doing identical ops to b */
				for (Uint32 i = 0; i < 4; ++i) {
					if(i != j) {
						ret.SubtractMultiplyRow(i, j, a.Get(j, i));
						a.SubtractMultiplyRow(i, j, a.Get(j, i));
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
	Vector3D<> GetTranslationVectorFrom(const Matrix4D<> & mat);
	Matrix4D<> GetRotationMatrixFrom(const Matrix4D<> & mat);
}
