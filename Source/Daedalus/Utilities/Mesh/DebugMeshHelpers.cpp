#include <Daedalus.h>
#include "DebugMeshHelpers.h"

namespace utils {
	inline void AddFace(
		std::vector<Triangle3D> & output,
		const Vector3D<> & f0,
		const Vector3D<> & f1,
		const Vector3D<> & f2,
		const Vector3D<> & f3
	) {
		output.push_back({ f0, f1, f3 });
		output.push_back({ f1, f2, f3 });
	}

	Uint16 CreatePrism(
		std::vector<Triangle3D> & output,
		Vector3D<> * const input
	) {
		AddFace(output, input[0], input[1], input[2], input[3]);
		AddFace(output, input[0], input[4], input[5], input[1]);
		AddFace(output, input[5], input[6], input[2], input[1]);
		AddFace(output, input[7], input[6], input[5], input[4]);
		AddFace(output, input[6], input[7], input[3], input[2]);
		AddFace(output, input[7], input[4], input[0], input[3]);

		return 12;
	}

	Vector3D<> CubeVertices[] = {
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
		std::vector<Triangle3D> & results, const Vector3D<> & position, const float radius
	) {
		
		Matrix4D<> transform =
			CreateTranslation(position) * CreateScaling({ radius, radius, radius });

		Vector3D<> transformedVertices[8];

		for (Uint16 i = 0; i < 8; i++)
			transformedVertices[i] = (transform * Vector4D<>(CubeVertices[i], 1)).Truncate();

		return CreatePrism(results, transformedVertices);
	}

	Uint16 CreateLine(
		std::vector<Triangle3D> & results, const Vector3D<> & startPoint,
		const Vector3D<> & endPoint, const float radius
	) {
		Vector3D<> u1 = endPoint - startPoint;
		Vector3D<> u2, u3;
		BuildBasis(u1, u2, u3);
		Matrix4D<> rotation = CreateRotation(u1, u2, u3);
		Matrix4D<> startTransform =
			CreateTranslation(startPoint) * rotation * CreateScaling({ radius, radius, radius });
		Matrix4D<> endTransform =
			CreateTranslation(endPoint) * rotation * CreateScaling({ radius, radius, radius });

		Vector3D<> transformedVertices[8];

		for (Uint16 i = 0; i < 8; i++) {
			if (CubeVertices[i].X < 0)
				transformedVertices[i] = (startTransform * Vector4D<>(CubeVertices[i], 1)).Truncate();
			else
				transformedVertices[i] = (endTransform * Vector4D<>(CubeVertices[i], 1)).Truncate();
		}
		
		return CreatePrism(results, transformedVertices);
	}
}
