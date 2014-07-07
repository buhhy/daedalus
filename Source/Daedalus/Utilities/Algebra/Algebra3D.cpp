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

	//bool Mesh::barycentric_coordinates(const Face & face, const Point3D & point, double &u, double &v, double &w) const {
	//	const Point3D & vert0 = m_verts[face.vinfo_list[0].vertex_index];
	//	const Point3D & vert1 = m_verts[face.vinfo_list[1].vertex_index];
	//	const Point3D & vert2 = m_verts[face.vinfo_list[2].vertex_index];

	//	Vector3D AC = vert2 - vert0;
	//	Vector3D CB = vert1 - vert2;
	//	Vector3D BA = vert0 - vert1;

	//	Vector3D normal = (-BA).cross(AC);
	//	u = normal.dot((point - vert0).cross(AC));
	//	if (u < 0)
	//		return false;
	//	v = normal.dot((point - vert1).cross(BA));
	//	if (v < 0)
	//		return false;
	//	w = normal.dot((point - vert2).cross(CB));
	//	if (w < 0)
	//		return false;

	//	double area2 = normal.dot(normal);
	//	u /= area2;
	//	v /= area2;
	//	w /= area2;

	//	return true;
	//}

}
