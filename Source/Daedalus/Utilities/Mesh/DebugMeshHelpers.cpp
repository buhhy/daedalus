#include <Daedalus.h>
#include "DebugMeshHelpers.h"

namespace utils {
	inline void AddFace(
		std::vector<Triangle> & output,
		const Vector3<> & f0,
		const Vector3<> & f1,
		const Vector3<> & f2,
		const Vector3<> & f3
	) {
		output.push_back({ f0, f1, f3 });
		output.push_back({ f1, f2, f3 });
	}

	Uint16 CreatePrism(
		std::vector<Triangle> & output,
		Vector3<> * const input
	) {
		AddFace(output, input[0], input[1], input[2], input[3]);
		AddFace(output, input[0], input[4], input[5], input[1]);
		AddFace(output, input[5], input[6], input[2], input[1]);
		AddFace(output, input[7], input[6], input[5], input[4]);
		AddFace(output, input[6], input[7], input[3], input[2]);
		AddFace(output, input[7], input[4], input[0], input[3]);

		return 12;
	}

	Vector3<> CubeVertices[] = {
		{ -1., -1., -1. },
		{ 1., -1., -1. },
		{ 1., 1., -1. },
		{ -1., 1., -1. },
		{ -1., -1., 1. },
		{ 1., -1., 1. },
		{ 1., 1., 1. },
		{ -1., 1., 1. }
	};

	Uint16 CreatePoint(
		std::vector<Triangle> & results, const Vector3<> & position, const float radius
	) {
		
		Matrix4<> transform =
			CreateTranslation(position) * CreateScaling({ radius, radius, radius });

		Vector3<> transformedVertices[8];

		for (Uint16 i = 0; i < 8; i++)
			transformedVertices[i] = (transform * Vector4<>(CubeVertices[i], 1)).Truncate();

		return CreatePrism(results, transformedVertices);
	}

	Uint16 CreateLine(
		std::vector<Triangle> & results, const Vector3<> & startPoint,
		const Vector3<> & endPoint, const float radius
	) {
		Vector3<> u1 = endPoint - startPoint;
		Vector3<> u2, u3;
		BuildBasis(u1, u2, u3);
		Matrix4<> rotation = CreateRotation(u1, u2, u3);
		Matrix4<> startTransform =
			CreateTranslation(startPoint) * rotation * CreateScaling({ radius, radius, radius });
		Matrix4<> endTransform =
			CreateTranslation(endPoint) * rotation * CreateScaling({ radius, radius, radius });

		Vector3<> transformedVertices[8];

		for (Uint16 i = 0; i < 8; i++) {
			if (CubeVertices[i].X < 0)
				transformedVertices[i] = (startTransform * Vector4<>(CubeVertices[i], 1)).Truncate();
			else
				transformedVertices[i] = (endTransform * Vector4<>(CubeVertices[i], 1)).Truncate();
		}
		
		return CreatePrism(results, transformedVertices);
	}
}
