// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GeneratedMeshComponent.generated.h"

USTRUCT(BlueprintType)
struct FMeshTriangleVertex {
	GENERATED_USTRUCT_BODY()

public:
	// Index of the material stored in the generated mesh
	UPROPERTY(EditAnywhere, Category = Vertex) uint32 MaterialIndex;
	UPROPERTY(EditAnywhere, Category = Vertex) FVector Position;
	UPROPERTY(EditAnywhere, Category = Vertex) UMaterialInterface * Material;
	UPROPERTY(EditAnywhere, Category = Vertex) FColor VertexColor;

	FMeshTriangleVertex() :
		Position(0.0), Material(NULL), VertexColor({ 255, 255, 255 }), MaterialIndex(0) {}

	FMeshTriangleVertex(const FVector & position) :
		Position(position),
		Material(NULL),
		VertexColor({ 255, 255, 255 }),
		MaterialIndex(0) {}

	FMeshTriangleVertex(
		const FVector & position,
		UMaterialInterface * const material
	) : Position(position),
		Material(material),
		VertexColor({ 255, 255, 255 }),
		MaterialIndex(0) {}

	FMeshTriangleVertex(
		const FVector & position,
		UMaterialInterface * const material,
		const FColor & vertexColor
	) : Position(position),
		Material(material),
		VertexColor(vertexColor),
		MaterialIndex(0) {}

};

USTRUCT(BlueprintType)
struct FMeshTriangle {
	GENERATED_USTRUCT_BODY()

public:
	// This property is set by the generated mesh component depending on the materials
	// present on the 3 vertices on the face
	UPROPERTY(EditAnywhere, Category = Materials) uint32 MaterialIndex;
	UPROPERTY(EditAnywhere, Category = Vertex) FMeshTriangleVertex Vertex0;
	UPROPERTY(EditAnywhere, Category = Vertex) FMeshTriangleVertex Vertex1;
	UPROPERTY(EditAnywhere, Category = Vertex) FMeshTriangleVertex Vertex2;

	FMeshTriangle() {}
	FMeshTriangle(
		const FMeshTriangleVertex & vertex0,
		const FMeshTriangleVertex & vertex1,
		const FMeshTriangleVertex & vertex2
	): Vertex0(vertex0), Vertex1(vertex1), Vertex2(vertex2) {}
};

/** Component that allows you to specify custom triangle mesh geometry */
UCLASS(editinlinenew, meta = (BlueprintSpawnableComponent), ClassGroup = Rendering)
class UGeneratedMeshComponent :
	public UMeshComponent, public IInterface_CollisionDataProvider {

	GENERATED_UCLASS_BODY()

public:
	/** Set the geometry to use on this triangle mesh */
	UFUNCTION(BlueprintCallable, Category = "Components|GeneratedMesh")
		bool SetGeneratedMeshTriangles(const TArray<FMeshTriangle>& Triangles);

	/** Set the geometry to use on this triangle mesh */
	UFUNCTION(BlueprintCallable, Category = "Components|GeneratedMesh")
		void ClearMeshTriangles();

	/** Description of collision */
	UPROPERTY(BlueprintReadOnly, Category = "Collision")
		class UBodySetup* ModelBodySetup;

	// Begin UMeshComponent interface.
	virtual int32 GetNumMaterials() const OVERRIDE;
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

	/** */
	TArray<FMeshTriangle> GeneratedMeshTris;

	friend class FGeneratedMeshSceneProxy;
};