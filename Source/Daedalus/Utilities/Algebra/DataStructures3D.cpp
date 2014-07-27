#include <Daedalus.h>
#include "DataStructures3D.h"

#include <algorithm>

#include <Utilities/Algebra/Algebra2D.h>
#include <Utilities/Algebra/Algebra3D.h>

namespace utils {
	bool AxisAlignedBoundingBox3D::FindIntersection(
		const Ray3D & ray,
		Point3D * intersectPoint,
		double * tValue
	) const {
		if (intersectPoint != NULL) *intersectPoint = ray.Origin;
		if (tValue != NULL) *tValue = 0;

		bool check = true;
		// There is an intersection if the ray originates within the bounding box.
		for (int i = 0; i < 3; i++) {
			if (ray.Origin[i] < MinPoint[i] || ray.Origin[i] > MaxPoint[i]) {
				check = false;
				break;
			}
		}

		if (check) return true;

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
}
