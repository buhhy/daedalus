#include <Daedalus.h>
#include <Utilities/DataStructures.h>
#include "Algebra2D.h"

namespace utils {
	Circle2D CalculateCircumcircle(
		const Vector2D<> & A,
		const Vector2D<> & B,
		const Vector2D<> & C
	) {
		// See http://en.wikipedia.org/wiki/Circumscribed_circle#Circumcenter_coordinates
		double AL = A.Length2();
		double BL = B.Length2();
		double CL = C.Length2();
		double D = 2 * (A.X * (B.Y - C.Y) + B.X * (C.Y - A.Y) + C.X * (A.Y - B.Y));
		double UX = (AL * (B.Y - C.Y) + BL * (C.Y - A.Y) + CL * (A.Y - B.Y)) / D;
		double UY = (AL * (C.X - B.X) + BL * (A.X - C.X) + CL * (B.X - A.X)) / D;
		double radius = std::sqrt((UX - A.X) * (UX - A.X) + (UY - A.Y) * (UY - A.Y));
		return Circle2D({ UX, UY }, radius);
	}

	Int8 IsWithinCircumcircle(const Vector2D<> & point, const Circle2D & circle) {
		auto result = (point - circle.Center).Length2() - (circle.Radius * circle.Radius);
		if (result < -FLOAT_ERROR) return 1;
		else if (result > FLOAT_ERROR) return -1;
		else return 0;
	}

	Int8 FindWinding(const Vector2D<> & p1, const Vector2D<> & pivot, const Vector2D<> & p3) {
		double res = (p1.X - pivot.X) * (p3.Y - pivot.Y) - (p1.Y - pivot.Y) * (p3.X - pivot.X);
		if (res < -FLOAT_ERROR) return -1;       // Obtuse
		if (res > FLOAT_ERROR) return 1;         // Acute
		else return 0;                              // Colinear
	}

	double FindAngle(const Vector2D<> & v1, const Vector2D<> & v2) {
		double result = std::atan2(v1.Determinant(v2), v1.Dot(v2));
		if (result < FLOAT_ERROR)
			result += 2 * MATH_PI;
		return result;
	}

	Vector2D<> projectVector2D(const Vector2D<> projection, const Vector2D<> target) {
		return target * (projection.Dot(target) / target.Length2());
	}

	void gramSchmidt2D(Vector2D<> & x, Vector2D<> & y) {
		Vector2D<> u1 = x;
		Vector2D<> u2 = y - projectVector2D(y, u1);
		x = u1.Normalize();
		y = u2.Normalize();
	}

	void buildBasis2D(Vector2D<> & input, Vector2D<> & y) {
		y.X = input.Y;
		y.Y = -input.X;
		gramSchmidt2D(input, y);
	}
}
