#pragma once

#include <Utilities/DataStructures.h>
#include <Utilities/Algebra/Vector3D.h>
#include <Utilities/Algebra/Vector4D.h>
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

	struct BoundingBox3D {
		virtual Basis3D GetBasis() const = 0;
		virtual Vector3D<> GetExtents() const = 0;
		virtual Vector3D<> GetCentre() const = 0;

		virtual bool BoundingBoxIntersection(
			const BoundingBox3D & box, const bool isInclusive = true) const;

		virtual bool RayIntersection(
			const Ray3D & ray, Point3D * intersectPoint = NULL, double * tValue = NULL) const = 0;

		virtual bool IsInside(const Point3D & point) const = 0;
	};

	struct AxisAlignedBoundingBox3D : BoundingBox3D {
		virtual Basis3D GetBasis() const;
		virtual Vector3D<> GetExtents() const;
		virtual Vector3D<> GetCentre() const;

		Point3D MinPoint;
		Point3D MaxPoint;

		AxisAlignedBoundingBox3D(const Point3D & min, const Point3D & max) :
			MinPoint(min), MaxPoint(max)
		{}

		/**
		 * This method WILL introduce floating point error into the intersection point. Make sure
		 * to do floating-point comparisons using epsilon.
		 */
		virtual bool RayIntersection(
			const Ray3D & ray, Point3D * intersectPoint = NULL, double * tValue = NULL) const;

		virtual bool IsInside(const Point3D & point) const;
	};

	/**
	 * Scaling transforms have an undefined behaviour on this bounding box, as it introduces
	 * shearing, which changes the bounding box to a non-box volume.
	 */
	struct OrientedBoundingBox3D : BoundingBox3D {
		virtual Basis3D GetBasis() const;
		virtual Vector3D<> GetExtents() const;
		virtual Vector3D<> GetCentre() const;

		AxisAlignedBoundingBox3D Bounds;
		Matrix4D<> Transform;

		OrientedBoundingBox3D(
			const Point3D & min, const Point3D & max,
			const Matrix4D<> & transform
		) : Bounds(min, max), Transform(transform)
		{}

		virtual bool RayIntersection(
			const Ray3D & ray, Point3D * intersectPoint = NULL, double * tValue = NULL) const;

		virtual bool IsInside(const Point3D & point) const;

		/**
		 * This returns the smallest bounding box that contains the transformed oriented box.
		 */
		AxisAlignedBoundingBox3D GetEnclosingBoundingBox() const;
	};



	// TODO: make actual colour class
	using Colour = Vector3D<Uint8>;

	// TODO: get rid of this
	struct GridCell {
		float values[8];
		Vector3D<> points[8];

		void Initialize(
			const float blf, const float tlf,
			const float blb, const float tlb,
			const float brf, const float trf,
			const float brb, const float trb
		) {
			values[0] = blf; values[1] = brf; values[2] = brb; values[3] = blb;
			values[4] = tlf; values[5] = trf; values[6] = trb; values[7] = tlb;
			points[0] = Vector3D<>(0, 0, 0);
			points[1] = Vector3D<>(1, 0, 0);
			points[2] = Vector3D<>(1, 1, 0);
			points[3] = Vector3D<>(0, 1, 0);
			points[4] = Vector3D<>(0, 0, 1);
			points[5] = Vector3D<>(1, 0, 1);
			points[6] = Vector3D<>(1, 1, 1);
			points[7] = Vector3D<>(0, 1, 1);
		}

		inline float Sum() const {
			float sum = 0;
			for (Uint8 i = 0; i < 8; i++)
				sum += values[i];
			return sum;
		}
	};
}
