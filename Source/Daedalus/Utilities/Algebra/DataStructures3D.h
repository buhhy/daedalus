#pragma once

#include <Utilities/DataStructures.h>
#include <Utilities/Algebra/Vector3D.h>
#include <Utilities/Algebra/Matrix4D.h>

#include <array>

namespace utils {
	using Point3D = Vector3D<>;

	struct Basis3D {
		Vector3D<> XVector, YVector, ZVector;

		Basis3D(const Vector3D<> & x);
		Basis3D(const Vector3D<> & x, const Vector3D<> & y);
		Basis3D(const Vector3D<> & x, const Vector3D<> & y, const Vector3D<> & z);

		Vector3D<> & operator [] (const Uint8 index) {
			switch (index) {
			case 0: return XVector;
			case 1: return YVector;
			case 2: return ZVector;
			default:
				std::stringstream ss;
				ss << "Basis3D::[]: Invalid index value `" << index << "`.";
				throw StringException(ss.str());
			}
		}

		const Vector3D<> & operator [] (const Uint8 index) const {
			switch (index) {
			case 0: return XVector;
			case 1: return YVector;
			case 2: return ZVector;
			default:
				std::stringstream ss;
				ss << "Basis3D::[]: Invalid index value `" << index << "`.";
				throw StringException(ss.str());
			}
		}
	};

	struct Triangle3D {
		Point3D Point1, Point2, Point3;

		Triangle3D(const Point3D & p1, const Point3D & p2, const Point3D & p3) :
			Point1(p1), Point2(p2), Point3(p3)
		{}
	};

	struct Ray3D {
		Point3D Origin;
		Vector3D<> Direction;

		Ray3D(const Point3D & origin, const Vector3D<> & direction) :
			Origin(origin), Direction(direction)
		{}
	};

	struct AxisAlignedBoundingBox3D {
	protected:
		virtual Basis3D GetBasis() const;
		virtual Vector3D<> GetExtents() const;
		virtual Vector3D<> GetCentre() const;
		//virtual std::array<Point3D, 8> GetPoints() const;

	public:
		Point3D MinPoint;
		Point3D MaxPoint;

		AxisAlignedBoundingBox3D(const Point3D & min, const Point3D & max) :
			MinPoint(min), MaxPoint(max)
		{}

		/**
		 * This method WILL introduce floating point error into the intersection point. Make sure
		 * to do floating-point comparisons using epsilon.
		 */
		bool FindIntersection(
			const Ray3D & ray, Point3D * intersectPoint = NULL, double * tValue = NULL) const;

		bool FindIntersection(
			const AxisAlignedBoundingBox3D & box, const bool isInclusive = true) const;
		
		bool IsInside(const Point3D & point) const;


	};

	struct OrientedBoundingBox3D : AxisAlignedBoundingBox3D {
	protected:
		virtual Basis3D GetBasis() const;
		virtual Vector3D<> GetCentre() const;
		//virtual std::array<Point3D, 8> GetPoints() const;

	public:
		Matrix4D<> Transform;

		OrientedBoundingBox3D(
			const Point3D & min, const Point3D & max,
			const Matrix4D<> & transform
		) : AxisAlignedBoundingBox3D(min, max), Transform(transform)
		{}
	};
}
