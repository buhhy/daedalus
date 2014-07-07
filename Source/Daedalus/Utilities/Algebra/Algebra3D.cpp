#include <Daedalus.h>
#include "Algebra3D.h"

namespace utils {
	Vector3D<> Project(const Vector3D<> projection, const Vector3D<> target) {
		return target * (projection.Dot(target) / target.Length2());
	}

	void GramSchmidt(Vector3D<> & x, Vector3D<> & y, Vector3D<> & z) {
		Vector3D<> u1 = x;
		Vector3D<> u2 = y - Project(y, u1);
		Vector3D<> u3 = z - Project(z, u1) - Project(z, u2);
		x = u1.Normalize();
		y = u2.Normalize();
		z = u3.Normalize();
	}

	void BuildBasis(Vector3D<> & input, Vector3D<> & y, Vector3D<> & z) {
		Vector3D<> u1 = input;
		Vector3D<> u2 = { 0, 0, 1. };
		Vector3D<> u3 = u1.Cross<double>(u2);

		// If the input is pointing up, we need to change the Z vector
		if (u3.Length2() <= FLOAT_ERROR) {
			u2 = { 1., 0, 0 };
			u3 = u1.Cross<double>(u2);
		}

		GramSchmidt(u1, u2, u3);
		input = u1; y = u2; z = u3;
	}

}
