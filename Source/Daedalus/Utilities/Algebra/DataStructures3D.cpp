#include <Daedalus.h>
#include "DataStructures3D.h"

#include <algorithm>

#include <Utilities/Algebra/Algebra2D.h>
#include <Utilities/Algebra/Algebra3D.h>

namespace utils {
	Basis3D::Basis3D(const Vector3D<> & x) : XVector(x) {
		BuildBasis(XVector, YVector, ZVector);
	}

	Basis3D::Basis3D(const Vector3D<> & x, const Vector3D<> & y) :
		XVector(x), YVector(y), ZVector(x.Cross<double>(y))
	{
		GramSchmidt(XVector, YVector, ZVector);
	}

	Basis3D::Basis3D(const Vector3D<> & x, const Vector3D<> & y, const Vector3D<> & z) :
		XVector(x), YVector(y), ZVector(z)
	{
		GramSchmidt(XVector, YVector, ZVector);
	}

	bool AxisAlignedBoundingBox3D::FindIntersection(
		const Ray3D & ray,
		Point3D * intersectPoint,
		double * tValue
	) const {
		if (intersectPoint != NULL) *intersectPoint = ray.Origin;
		if (tValue != NULL) *tValue = 0;
		
		// There is an intersection if the ray originates within the bounding box.
		if (IsInside(ray.Origin))
			return true;

		double txmin = 0;
		double txmax = 0;

		double tymin = 0;
		double tymax = 0;

		double tzmin = 0;
		double tzmax = 0;

		double divx = 1.0/ray.Direction.X;
		if (divx > 0) {
			txmin = (MinPoint.X - ray.Origin.X) * divx;
			txmax = (MaxPoint.X - ray.Origin.X) * divx;
		} else {
			txmax = (MinPoint.X - ray.Origin.X) * divx;
			txmin = (MaxPoint.X - ray.Origin.X) * divx;
		}

		double divy = 1.0/ray.Direction.Y;
		if (divy > 0) {
			tymin = (MinPoint.Y - ray.Origin.Y) * divy;
			tymax = (MaxPoint.Y - ray.Origin.Y) * divy;
		} else {
			tymax = (MinPoint.Y - ray.Origin.Y) * divy;
			tymin = (MaxPoint.Y - ray.Origin.Y) * divy;
		}

		if (txmin > tymax || txmax < tymin)
			return false;

		double tmin = std::max(txmin, tymin);
		double tmax = std::min(txmax, tymax);

		double divz = 1.0/ray.Direction.Z;
		if (divz > 0) {
			tzmin = (MinPoint.Z - ray.Origin.Z) * divz;
			tzmax = (MaxPoint.Z - ray.Origin.Z) * divz;
		} else {
			tzmax = (MinPoint.Z - ray.Origin.Z) * divz;
			tzmin = (MaxPoint.Z - ray.Origin.Z) * divz;
		}

		if (tmin > tzmax || tmax < tzmin)
			return false;

		tmin = std::max(tmin, tzmin);
		tmax = std::min(tmax, tzmax);

		if (tmin <= 0)
			return false;

		if (intersectPoint != NULL) *intersectPoint = ray.Origin + ray.Direction * tmin;
		if (tValue != NULL) *tValue = tmin;

		return true;
	}
	
	bool AxisAlignedBoundingBox3D::IsInside(const Point3D & point) const {
		for (int i = 0; i < 3; i++) {
			if (point[i] < MinPoint[i] || point[i] > MaxPoint[i])
				return false;
		}
		return true;
	}

	Vector3D<> AxisAlignedBoundingBox3D::GetExtents() const {
		return (MaxPoint - MinPoint) / 2.0;
	}

	Vector3D<> AxisAlignedBoundingBox3D::GetCentre() const {
		return (MaxPoint + MinPoint) / 2.0;
	}

	Vector3D<> OrientedBoundingBox3D::GetCentre() const {
		return Transform * ((MaxPoint + MinPoint) / 2.0);
	}
	
	bool AxisAlignedBoundingBox3D::FindIntersection(
		const AxisAlignedBoundingBox3D & box,
		const bool isInclusive
	) const {
		// Algorithm based on this paper:
		// http://www.geometrictools.com/Documentation/DynamicCollisionDetection.pdf
		const auto compare = [&] (const double r1, const double r2) {
			return isInclusive && EGT(r1, r2) || !isInclusive && EGTE(r1, r2);
		};

		// Cutoff for cosine of angles between box axes.  This is used to catch
		// the cases when at least one pair of axes are parallel.  If this
		// happens, there is no need to test for separation along the
		// Cross(thisBasis[i], thatExtents[j]) directions.
		const double cutoff = 1.0 - DOUBLE_RADIAN_EPSILON;
		bool existsParallelPair = false;

		// Convenience variables.
		const Basis3D thisBasis = GetBasis();
		const Basis3D otherBasis = box.GetBasis();
		const Vector3D<> thisExtents = GetExtents();
		const Vector3D<> thatExtents = box.GetExtents();

		// Compute difference of box centers, centreDiff = C1-C0.
		Vector3D<> centreDiff = box.GetCentre() - GetCentre();

		double C[3][3];     // matrix C = A^T B, c_{ij} = Dot(thisBasis_i, otherBasis_j)
		double AbsC[3][3];  // |c_{ij}|
		double AD[3];       // Dot(thisBasis_i, centreDiff)
		double thisRadius, thatRadius, radiusDiff; // interval radii and distance between centers
		double radiusSum;         // = thisRadius + thatRadius

		// axis C0+t*A0
		for (Uint8 i = 0; i < 3; ++i) {
			C[0][i] = thisBasis.XVector.Dot(otherBasis[i]);
			AbsC[0][i] = std::abs(C[0][i]);
			if (AbsC[0][i] > cutoff)
				existsParallelPair = true;
		}
	
		AD[0] = thisBasis.XVector.Dot(centreDiff);
		radiusDiff = std::abs(AD[0]);
		thatRadius = thatExtents[0]*AbsC[0][0] + thatExtents[1]*AbsC[0][1] + thatExtents[2]*AbsC[0][2];
		radiusSum = thisExtents[0] + thatRadius;
		if (compare(radiusDiff, radiusSum))
			return false;

		// axis C0+t*A1
		for (Uint8 i = 0; i < 3; ++i) {
			C[1][i] = thisBasis.YVector.Dot(otherBasis[i]);
			AbsC[1][i] = std::abs(C[1][i]);
			if (AbsC[1][i] > cutoff)
				existsParallelPair = true;
		}
		AD[1] = thisBasis.YVector.Dot(centreDiff);
		radiusDiff = std::abs(AD[1]);
		thatRadius = thatExtents[0]*AbsC[1][0] + thatExtents[1]*AbsC[1][1] + thatExtents[2]*AbsC[1][2];
		radiusSum = thisExtents[1] + thatRadius;
		if (compare(radiusDiff, radiusSum))
			return false;

		// axis C0+t*A2
		for (Uint8 i = 0; i < 3; ++i) {
			C[2][i] = thisBasis.ZVector.Dot(otherBasis[i]);
			AbsC[2][i] = std::abs(C[2][i]);
			if (AbsC[2][i] > cutoff)
				existsParallelPair = true;
		}
		AD[2] = thisBasis.ZVector.Dot(centreDiff);
		radiusDiff = std::abs(AD[2]);
		thatRadius = thatExtents[0]*AbsC[2][0] + thatExtents[1]*AbsC[2][1] + thatExtents[2]*AbsC[2][2];
		radiusSum = thisExtents[2] + thatRadius;
		if (compare(radiusDiff, radiusSum))
			return false;

		// axis C0+t*B0
		radiusDiff = std::abs(otherBasis.XVector.Dot(centreDiff));
		thisRadius = thisExtents[0]*AbsC[0][0] + thisExtents[1]*AbsC[1][0] + thisExtents[2]*AbsC[2][0];
		radiusSum = thisRadius + thatExtents[0];
		if (compare(radiusDiff, radiusSum))
			return false;

		// axis C0+t*B1
		radiusDiff = std::abs(otherBasis.YVector.Dot(centreDiff));
		thisRadius = thisExtents[0]*AbsC[0][1] + thisExtents[1]*AbsC[1][1] + thisExtents[2]*AbsC[2][1];
		radiusSum = thisRadius + thatExtents[1];
		if (compare(radiusDiff, radiusSum))
			return false;

		// axis C0+t*B2
		radiusDiff = std::abs(otherBasis.ZVector.Dot(centreDiff));
		thisRadius = thisExtents[0]*AbsC[0][2] + thisExtents[1]*AbsC[1][2] + thisExtents[2]*AbsC[2][2];
		radiusSum = thisRadius + thatExtents[2];
		if (compare(radiusDiff, radiusSum))
			return false;

		// At least one pair of box axes was parallel, so the separation is
		// effectively in 2D where checking the "edge" normals is sufficient for
		// the separation of the boxes.
		if (existsParallelPair)
			return true;

		// axis C0+t*A0xB0
		radiusDiff = std::abs(AD[2]*C[1][0] - AD[1]*C[2][0]);
		thisRadius = thisExtents[1]*AbsC[2][0] + thisExtents[2]*AbsC[1][0];
		thatRadius = thatExtents[1]*AbsC[0][2] + thatExtents[2]*AbsC[0][1];
		radiusSum = thisRadius + thatRadius;
		if (compare(radiusDiff, radiusSum))
			return false;

		// axis C0+t*A0xB1
		radiusDiff = std::abs(AD[2]*C[1][1] - AD[1]*C[2][1]);
		thisRadius = thisExtents[1]*AbsC[2][1] + thisExtents[2]*AbsC[1][1];
		thatRadius = thatExtents[0]*AbsC[0][2] + thatExtents[2]*AbsC[0][0];
		radiusSum = thisRadius + thatRadius;
		if (compare(radiusDiff, radiusSum))
			return false;

		// axis C0+t*A0xB2
		radiusDiff = std::abs(AD[2]*C[1][2] - AD[1]*C[2][2]);
		thisRadius = thisExtents[1]*AbsC[2][2] + thisExtents[2]*AbsC[1][2];
		thatRadius = thatExtents[0]*AbsC[0][1] + thatExtents[1]*AbsC[0][0];
		radiusSum = thisRadius + thatRadius;
		if (compare(radiusDiff, radiusSum))
			return false;

		// axis C0+t*A1xB0
		radiusDiff = std::abs(AD[0]*C[2][0] - AD[2]*C[0][0]);
		thisRadius = thisExtents[0]*AbsC[2][0] + thisExtents[2]*AbsC[0][0];
		thatRadius = thatExtents[1]*AbsC[1][2] + thatExtents[2]*AbsC[1][1];
		radiusSum = thisRadius + thatRadius;
		if (compare(radiusDiff, radiusSum))
			return false;

		// axis C0+t*A1xB1
		radiusDiff = std::abs(AD[0]*C[2][1] - AD[2]*C[0][1]);
		thisRadius = thisExtents[0]*AbsC[2][1] + thisExtents[2]*AbsC[0][1];
		thatRadius = thatExtents[0]*AbsC[1][2] + thatExtents[2]*AbsC[1][0];
		radiusSum = thisRadius + thatRadius;
		if (compare(radiusDiff, radiusSum))
			return false;

		// axis C0+t*A1xB2
		radiusDiff = std::abs(AD[0]*C[2][2] - AD[2]*C[0][2]);
		thisRadius = thisExtents[0]*AbsC[2][2] + thisExtents[2]*AbsC[0][2];
		thatRadius = thatExtents[0]*AbsC[1][1] + thatExtents[1]*AbsC[1][0];
		radiusSum = thisRadius + thatRadius;
		if (compare(radiusDiff, radiusSum))
			return false;

		// axis C0+t*A2xB0
		radiusDiff = std::abs(AD[1]*C[0][0] - AD[0]*C[1][0]);
		thisRadius = thisExtents[0]*AbsC[1][0] + thisExtents[1]*AbsC[0][0];
		thatRadius = thatExtents[1]*AbsC[2][2] + thatExtents[2]*AbsC[2][1];
		radiusSum = thisRadius + thatRadius;
		if (compare(radiusDiff, radiusSum))
			return false;

		// axis C0+t*A2xB1
		radiusDiff = std::abs(AD[1]*C[0][1] - AD[0]*C[1][1]);
		thisRadius = thisExtents[0]*AbsC[1][1] + thisExtents[1]*AbsC[0][1];
		thatRadius = thatExtents[0]*AbsC[2][2] + thatExtents[2]*AbsC[2][0];
		radiusSum = thisRadius + thatRadius;
		if (compare(radiusDiff, radiusSum))
			return false;

		// axis C0+t*A2xB2
		radiusDiff = std::abs(AD[1]*C[0][2] - AD[0]*C[1][2]);
		thisRadius = thisExtents[0]*AbsC[1][2] + thisExtents[1]*AbsC[0][2];
		thatRadius = thatExtents[0]*AbsC[2][1] + thatExtents[1]*AbsC[2][0];
		radiusSum = thisRadius + thatRadius;
		if (compare(radiusDiff, radiusSum))
			return false;

		return true;
	}
	
	Basis3D AxisAlignedBoundingBox3D::GetBasis() const {
		return Basis3D(Vector3D<>(1, 0, 0), Vector3D<>(0, 1, 0), Vector3D<>(0, 0, 1));
	}
	
	Basis3D OrientedBoundingBox3D::GetBasis() const {
		return GetBasisFrom(Transform);
	}
	
	/*std::array<Point3D, 8> AxisAlignedBoundingBox3D::GetPoints() const {
		return {{
			this->MinPoint,
			{ this->MaxPoint.X, this->MinPoint.Y, this->MinPoint.Z },
			{ this->MinPoint.X, this->MaxPoint.Y, this->MinPoint.Z },
			{ this->MaxPoint.X, this->MaxPoint.Y, this->MinPoint.Z },
			{ this->MinPoint.X, this->MinPoint.Y, this->MaxPoint.Z },
			{ this->MaxPoint.X, this->MinPoint.Y, this->MaxPoint.Z },
			{ this->MinPoint.X, this->MaxPoint.Y, this->MaxPoint.Z },
			this->MaxPoint
		}};
	}
		
	std::array<Point3D, 8> OrientedBoundingBox3D::GetPoints() const {
		return {{
			Transform * this->MinPoint,
			Transform * { this->MaxPoint.X, this->MinPoint.Y, this->MinPoint.Z },
			Transform * { this->MinPoint.X, this->MaxPoint.Y, this->MinPoint.Z },
			Transform * { this->MaxPoint.X, this->MaxPoint.Y, this->MinPoint.Z },
			Transform * { this->MinPoint.X, this->MinPoint.Y, this->MaxPoint.Z },
			Transform * { this->MaxPoint.X, this->MinPoint.Y, this->MaxPoint.Z },
			Transform * { this->MinPoint.X, this->MaxPoint.Y, this->MaxPoint.Z },
			Transform * this->MaxPoint
		}};
	}*/
}
