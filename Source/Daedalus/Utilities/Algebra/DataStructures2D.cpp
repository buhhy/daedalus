#include <Daedalus.h>
#include "DataStructures2D.h"

#include <Utilities/Algebra/Algebra2D.h>
#include <Utilities/Algebra/Algebra3D.h>

namespace utils {
	bool Triangle2D::GetBarycentricCoordinates(
		UVWVector & output, const Vector2D<> & point
	) const {
		// To use the UVW values, use this formula:
		// ivalue = u * v2 + v * v3 + w * v1
		// X is aliased to u, Y to v, Z to w
		Vector3D<> AC = Point3 - Point1;
		Vector3D<> CB = Point2 - Point3;
		Vector3D<> BA = Point1 - Point2;

		auto normal = (-BA).Cross<double>(AC);
		output.X = normal.Dot(Vector3D<>(point - Point1).Cross<double>(AC));
		if (output.X < 0) return false;

		output.Y = normal.Dot(Vector3D<>(point - Point2).Cross<double>(BA));
		if (output.Y < 0) return false;

		output.Z = normal.Dot(Vector3D<>(point - Point3).Cross<double>(CB));
		if (output.Z < 0) return false;

		double area2 = normal.Dot(normal);
		output /= area2;

		return true;
	}



	/********************************************************************************
	 * Basis 2D
	 ********************************************************************************/

	Basis2D::Basis2D(const Vector2D<> & x) : xVector(x) {
		buildBasis2D(xVector, yVector);
	}

	Basis2D::Basis2D(const Vector2D<> & x, const Vector2D<> & y) :
		xVector(x), yVector(y)
	{
		gramSchmidt2D(xVector, yVector);
	}

	Vector2D<> & Basis2D::operator [] (const Uint8 index) {
		switch (index) {
		case 0: return xVector;
		case 1: return yVector;
		default:
			std::stringstream ss;
			ss << "Basis2D::[] const: Invalid index value `" << index << "`.";
			throw StringException(ss.str());
		}
	}

	const Vector2D<> & Basis2D::operator [] (const Uint8 index) const {
		switch (index) {
		case 0: return xVector;
		case 1: return yVector;
		default:
			std::stringstream ss;
			ss << "Basis2D::[] const: Invalid index value `" << index << "`.";
			throw StringException(ss.str());
		}
	}

	bool AxisAlignedBoundingBox2D::isInside(const Point2D & point) const {
		for (int i = 0; i < 2; i++) {
			if (point[i] < minPoint[i] || point[i] > maxPoint[i])
				return false;
		}
		return true;
	}

	Vector2D<> AxisAlignedBoundingBox2D::getExtents() const {
		return (maxPoint - minPoint) / 2.0;
	}

	Vector2D<> AxisAlignedBoundingBox2D::getCentre() const {
		return (maxPoint + minPoint) / 2.0;
	}
	
	Basis2D AxisAlignedBoundingBox2D::getBasis() const {
		return Basis2D(Vector2D<>(1, 0), Vector2D<>(0, 1));
	}
}
