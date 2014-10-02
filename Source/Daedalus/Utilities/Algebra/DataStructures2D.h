#pragma once

#include <Utilities/Algebra/Vector2D.h>
#include <Utilities/Algebra/Vector3D.h>
#include <Utilities/Algebra/Tensor2D.h>

namespace utils {
	using UVWVector = Vector3D<>;
	using Point2D = Vector2D<>;

	struct Basis2D {
		Vector2D<> xVector, yVector;

		Basis2D(const Vector2D<> & x);
		Basis2D(const Vector2D<> & x, const Vector2D<> & y);

		Vector2D<> & operator [] (const Uint8 index);
		const Vector2D<> & operator [] (const Uint8 index) const;
	};

	struct Circle2D {
		Point2D Center;
		double Radius;

		Circle2D(const Point2D center, const double radius) :
			Center(center), Radius(radius)
		{}
		Circle2D() : Circle2D({ 0, 0 }, 0) {}
	};

	struct Triangle2D {
		Point2D Point1, Point2, Point3;

		Triangle2D(const Point2D & p1, const Point2D & p2, const Point2D & p3) :
			Point1(p1), Point2(p2), Point3(p3)
		{}

		/**
		 * Retrieves the barycentric coordinates of the provided point.
		 * @Return true if the point is within this triangle, false if not
		 */
		bool GetBarycentricCoordinates(UVWVector & output, const Point2D & point) const;
	};

	/**
	 * A 2D rectangle represented by an origin point and its size.
	 */
	struct Box2D {
		Point2D origin;
		Vector2D<> size;

		Box2D(const Point2D & origin, const Vector2D<> size) :
			origin(origin), size(size)
		{}

		bool isInside(const Point2D & point) const;
	};

	struct BoundingBox2D {
		virtual Basis2D getBasis() const = 0;
		virtual Vector2D<> getExtents() const = 0;
		virtual Vector2D<> getCentre() const = 0;

		//virtual bool boundingBoxIntersection(
		//	const BoundingBox2D & box, const bool isInclusive = true) const;

		virtual bool isInside(const Point2D & point) const = 0;
	};

	struct AxisAlignedBoundingBox2D : BoundingBox2D {
		Point2D minPoint;
		Point2D maxPoint;



		AxisAlignedBoundingBox2D() : minPoint(0), maxPoint(0) {}
		AxisAlignedBoundingBox2D(const Box2D & box) :
			minPoint(box.origin), maxPoint(box.origin.X + box.size.X, box.origin.Y + box.size.Y)
		{}
		AxisAlignedBoundingBox2D(const Point2D & min, const Point2D & max) :
			minPoint(min), maxPoint(max)
		{}
		
		virtual Basis2D getBasis() const;
		virtual Vector2D<> getExtents() const;
		virtual Vector2D<> getCentre() const;

		virtual bool isInside(const Point2D & point) const;
	};
}
