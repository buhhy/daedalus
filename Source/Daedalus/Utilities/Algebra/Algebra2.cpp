#include "Daedalus.h"
#include "DataStructures.h"
#include "Algebra2.h"

namespace utils {
	Circle2D CalculateCircumcircle(
		const Vector2<> & A,
		const Vector2<> & B,
		const Vector2<> & C
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

	int8 IsWithinCircumcircle(const Vector2<> & point, const Circle2D & circle) {
		auto result = (point - circle.Center).Length2() - (circle.Radius * circle.Radius);
		if (result < -FLOAT_ERROR) return 1;
		else if (result > FLOAT_ERROR) return -1;
		else return 0;
	}

	int8 FindWinding(const Vector2<> & p1, const Vector2<> & pivot, const Vector2<> & p3) {
		double result = (p1 - pivot).Determinant(p3 - pivot);
		if (result < -FLOAT_ERROR) return -1;       // Obtuse
		if (result > FLOAT_ERROR) return 1;         // Acute
		else return 0;                              // Colinear
	}

	float FindAngle(const Vector2<> & v1, const Vector2<> & v2) {
		float result = FMath::Atan2(v1.Determinant(v2), v1.Dot(v2));
		if (result < 0)
			result += 2 * M_PI;
		return result;
	}
}
