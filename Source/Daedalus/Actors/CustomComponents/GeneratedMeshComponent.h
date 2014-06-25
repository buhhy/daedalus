// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once

#include <Utilities/UnrealBridge.h>
#include "GeneratedMeshComponent.generated.h"

/**
 * This data structure contains all the information for a vertex including position, vertex
 * color as well as material information.
 */
USTRUCT(BlueprintType)
struct FMeshTriangleVertex {
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, Category = Vertex) FVector Position;
	UPROPERTY(EditAnywhere, Category = Vertex) UMaterialInterface * Material;
	UPROPERTY(EditAnywhere, Category = Vertex) FColor VertexColor;

	FMeshTriangleVertex() :
		Position(0.0), Material(NULL), VertexColor({ 255, 255, 255 }) {}

	FMeshTriangleVertex(const FVector & position) :
		Position(position),
		Material(NULL),
		VertexColor({ 255, 255, 255 }) {}

	FMeshTriangleVertex(
		const utils::Vector3<> & position,
		UMaterialInterface * const material = NULL
	) : FMeshTriangleVertex(utils::ToFVector(position), material) {}

	FMeshTriangleVertex(
		const FVector & position,
		UMaterialInterface * const material
	) : Position(position),
		Material(material),
		VertexColor({ 255, 255, 255 }) {}

	FMeshTriangleVertex(
		const FVector & position,
		UMaterialInterface * const material,
		const FColor & vertexColor
	) : Position(position),
		Material(material),
		VertexColor(vertexColor) {}

};

USTRUCT(BlueprintType)
struct FMeshTriangle {
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, Category = Vertex) FMeshTriangleVertex Vertex0;
	UPROPERTY(EditAnywhere, Category = Vertex) FMeshTriangleVertex Vertex1;
	UPROPERTY(EditAnywhere, Category = Vertex) FMeshTriangleVertex Vertex2;

	FMeshTriangle() {}
	FMeshTriangle(
		const FMeshTriangleVertex & vertex0,
		const FMeshTriangleVertex & vertex1,
		const FMeshTriangleVertex & vertex2
	) : Vertex0(vertex0), Vertex1(vertex1), Vertex2(vertex2) {}
	FMeshTriangle(const utils::Triangle & tri, UMaterialInterface * const material) :
		FMeshTriangle(
			FMeshTriangleVertex(tri.Point1, material),
			FMeshTriangleVertex(tri.Point2, material),
			FMeshTriangleVertex(tri.Point3, material)) {}
};

/** Component that allows you to specify custom triangle mesh geometry */
UCLASS(editinlinenew, meta = (BlueprintSpawnableComponent), ClassGroup = Rendering)
class UGeneratedMeshComponent :
	public UMeshComponent, public IInterface_CollisionDataProvider {

	GENERATED_UCLASS_BODY()

public:
	/** Set the geometry to use on this triangle mesh */
	UFUNCTION(BlueprintCallable, Category = "Components|GeneratedMesh")
		bool SetGeneratedMeshTriangles(const TArray<FMeshTriangle> & Triangles);

	/** Set the geometry to use on this triangle mesh */
	UFUNCTION(BlueprintCallable, Category = "Components|GeneratedMesh")
		void ClearMeshTriangles();

	/** Description of collision */
	UPROPERTY(BlueprintReadOnly, Category = "Collision")
	class UBodySetup* ModelBodySetup;

	// Begin UMeshComponent interface.
	virtual int32 GetNumMaterials() const OVERRIDE;
	virtual UMaterialInterface * GetMaterial(int32 ElementIndex) const OVERRIDE;
	// End UMeshComponent interface.

	// Begin Interface_CollisionDataProvider Interface
	virtual bool GetPhysicsTriMeshData(struct FTriMeshCollisionData* CollisionData, bool InUseAllTriData) OVERRIDE;
	virtual bool ContainsPhysicsTriMeshData(bool InUseAllTriData) const OVERRIDE;
	virtual bool WantsNegXTriMesh() OVERRIDE{ return false; }
	// End Interface_CollisionDataProvider Interface

	// Begin UPrimitiveComponent interface.
	virtual FPrimitiveSceneProxy* CreateSceneProxy() OVERRIDE;
	virtual class UBodySetup* GetBodySetup() OVERRIDE;
	// End UPrimitiveComponent interface.

	void UpdateBodySetup();
	void UpdateCollision();

private:
	// Begin USceneComponent interface.
	virtual FBoxSphereBounds CalcBounds(const FTransform & LocalToWorld) const OVERRIDE;
	// Begin USceneComponent interface.

	// Mesh triangles mapped by material IDs, since only 1 material can be applied to
	// each mesh group, we need to create a mesh group for every material present in
	// the entire mesh.
	TMap<uint32, TArray<FMeshTriangle> > MeshTriangles;

	// Cache count of the contents of MeshTriangles
	uint64 MeshTriangleCount;

	friend class FGeneratedMeshSceneProxy;
};