#include "Daedalus.h"
#include "TestMesh.h"

inline FMeshTriangleVertex WithMaterial(
	const FMeshTriangleVertex & base,
	UMaterialInterface * const material
) {
	FMeshTriangleVertex temp(base);
	temp.Material = material;
	return temp;
}

inline UMaterialInstanceDynamic * ColoredMaterial(
	UMaterial * base,
	UObject * ref,
	const FLinearColor & color
) {
	auto mat = UMaterialInstanceDynamic::Create((UMaterial *) base, ref);
	mat->SetVectorParameterValue(FName(TEXT("TestProperty")), color);
	return mat;
}

inline void AddFace(
	TArray<FMeshTriangle> & output,
	const FMeshTriangleVertex & f0,
	const FMeshTriangleVertex & f1,
	const FMeshTriangleVertex & f2,
	const FMeshTriangleVertex & f3
) {
	output.Add({ f0, f1, f3 });
	output.Add({ f1, f2, f3 });
}

ATestMesh::ATestMesh(const class FPostConstructInitializeProperties & PCIP)
	: Super(PCIP) {

	Mesh = PCIP.CreateDefaultSubobject<UGeneratedMeshComponent>(this, TEXT("GeneratedMesh"));


	ConstructorHelpers::FObjectFinder<UMaterial> testMaterial(
		TEXT("Material'/Game/TestMaterial.TestMaterial'"));
	auto redMat = UMaterialInstanceDynamic::Create((UMaterial *) testMaterial.Object, this);
	redMat->SetVectorParameterValue(FName(TEXT("TestProperty")), FLinearColor(1.0, 0.0, 0.0, 1.0));

	UMaterialInstanceDynamic * colors[] = {
		ColoredMaterial(testMaterial.Object, this, FLinearColor(1.0, 0.0, 0.0, 1.0)),
		ColoredMaterial(testMaterial.Object, this, FLinearColor(0.0, 1.0, 0.0, 1.0)),
		ColoredMaterial(testMaterial.Object, this, FLinearColor(0.0, 0.0, 1.0, 1.0)),
		ColoredMaterial(testMaterial.Object, this, FLinearColor(1.0, 1.0, 0.0, 1.0)),
		ColoredMaterial(testMaterial.Object, this, FLinearColor(0.0, 1.0, 1.0, 1.0)),
		ColoredMaterial(testMaterial.Object, this, FLinearColor(1.0, 0.0, 1.0, 1.0)),
	};

	FMeshTriangleVertex verts[] = {
		{ FVector(-50, -50, -50) },
		{ FVector(50, -50, -50) },
		{ FVector(50, 50, -50) },
		{ FVector(-50, 50, -50) },
		{ FVector(-50, -50, 50) },
		{ FVector(50, -50, 50) },
		{ FVector(50, 50, 50) },
		{ FVector(-50, 50, 50) }
	};

	TArray<FMeshTriangle> triangles;

	AddFace(triangles,
			WithMaterial(verts[0], colors[0]),
			WithMaterial(verts[1], colors[0]),
			WithMaterial(verts[2], colors[0]),
			WithMaterial(verts[3], colors[0]));

	AddFace(triangles,
			WithMaterial(verts[0], colors[1]),
			WithMaterial(verts[4], colors[1]),
			WithMaterial(verts[5], colors[1]),
			WithMaterial(verts[1], colors[1]));

	AddFace(triangles,
			WithMaterial(verts[5], colors[2]),
			WithMaterial(verts[6], colors[2]),
			WithMaterial(verts[2], colors[2]),
			WithMaterial(verts[1], colors[2]));

	AddFace(triangles,
			WithMaterial(verts[7], colors[3]),
			WithMaterial(verts[6], colors[3]),
			WithMaterial(verts[5], colors[3]),
			WithMaterial(verts[4], colors[3]));

	AddFace(triangles,
			WithMaterial(verts[6], colors[4]),
			WithMaterial(verts[7], colors[4]),
			WithMaterial(verts[3], colors[4]),
			WithMaterial(verts[2], colors[4]));

	AddFace(triangles,
			WithMaterial(verts[7], colors[5]),
			WithMaterial(verts[4], colors[5]),
			WithMaterial(verts[0], colors[5]),
			WithMaterial(verts[3], colors[5]));

	Mesh->SetGeneratedMeshTriangles(triangles);

	RootComponent = Mesh;
}
