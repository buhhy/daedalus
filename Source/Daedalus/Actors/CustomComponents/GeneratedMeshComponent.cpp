// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved. 
#include "Daedalus.h"
#include "GeneratedMeshComponent.h"

/////////////////////////////////////////////////////////////////////////////
// Private Classes
/////////////////////////////////////////////////////////////////////////////

/** Vertex Buffer */
class FGeneratedMeshVertexBuffer : public FVertexBuffer {
public:
	TArray<FDynamicMeshVertex> Vertices;

	virtual void InitRHI() {
		VertexBufferRHI = RHICreateVertexBuffer(
			Vertices.Num() * sizeof(FDynamicMeshVertex), NULL, BUF_Static);

		// Copy the vertex data into the vertex buffer.
		void * VertexBufferData = RHILockVertexBuffer(
			VertexBufferRHI, 0, Vertices.Num() * sizeof(FDynamicMeshVertex), RLM_WriteOnly);
		FMemory::Memcpy(
			VertexBufferData,
			Vertices.GetTypedData(),
			Vertices.Num() * sizeof(FDynamicMeshVertex));
		RHIUnlockVertexBuffer(VertexBufferRHI);
	}

};

/** Index Buffer */
class FGeneratedMeshIndexBuffer : public FIndexBuffer {
public:
	TArray<int32> Indices;

	virtual void InitRHI() {
		IndexBufferRHI = RHICreateIndexBuffer(
			sizeof(int32), Indices.Num() * sizeof(int32), NULL, BUF_Static);

		// Write the indices to the index buffer.
		void* Buffer = RHILockIndexBuffer(
			IndexBufferRHI, 0, Indices.Num() * sizeof(int32), RLM_WriteOnly);
		FMemory::Memcpy(Buffer, Indices.GetTypedData(), Indices.Num() * sizeof(int32));
		RHIUnlockIndexBuffer(IndexBufferRHI);
	}
};

/** Vertex Factory */
class FGeneratedMeshVertexFactory : public FLocalVertexFactory {
public:
	FGeneratedMeshVertexFactory() {}

	/** Initialization */
	void Init(const FGeneratedMeshVertexBuffer * VertexBuffer) {
		check(!IsInRenderingThread());

		ENQUEUE_UNIQUE_RENDER_COMMAND_TWOPARAMETER(
			InitGeneratedMeshVertexFactory,
			FGeneratedMeshVertexFactory *, VertexFactory, this,
			const FGeneratedMeshVertexBuffer*, VertexBuffer, VertexBuffer,
			{
				// Initialize the vertex factory's stream components.
				DataType NewData;
				NewData.PositionComponent = STRUCTMEMBER_VERTEXSTREAMCOMPONENT(
					VertexBuffer, FDynamicMeshVertex, Position, VET_Float3);
				NewData.TextureCoordinates.Add(
					FVertexStreamComponent(
						VertexBuffer,
						STRUCT_OFFSET(FDynamicMeshVertex, TextureCoordinate),
						sizeof(FDynamicMeshVertex), VET_Float2));

				NewData.TangentBasisComponents[0] = STRUCTMEMBER_VERTEXSTREAMCOMPONENT(
					VertexBuffer, FDynamicMeshVertex, TangentX, VET_PackedNormal);
				NewData.TangentBasisComponents[1] = STRUCTMEMBER_VERTEXSTREAMCOMPONENT(
					VertexBuffer, FDynamicMeshVertex, TangentZ, VET_PackedNormal);
				NewData.ColorComponent = STRUCTMEMBER_VERTEXSTREAMCOMPONENT(
					VertexBuffer, FDynamicMeshVertex, Color, VET_Color);
				VertexFactory->SetData(NewData);
			});
	}
};

struct FMeshGroup {
	UMaterialInterface * Material;
	FGeneratedMeshVertexBuffer VertexBuffer;
	FGeneratedMeshIndexBuffer IndexBuffer;
	FGeneratedMeshVertexFactory VertexFactory;

	void InitializeResources() {
		// Init vertex factory
		VertexFactory.Init(&VertexBuffer);

		// Enqueue initialization of render resource
		BeginInitResource(&VertexBuffer);
		BeginInitResource(&IndexBuffer);
		BeginInitResource(&VertexFactory);
	}

	void ReleaseResources() {
		VertexBuffer.ReleaseResource();
		IndexBuffer.ReleaseResource();
		VertexFactory.ReleaseResource();
		Material = NULL;
	}

	inline void BuildVertex(
		const FMeshTriangleVertex & vertex,
		const FVector & tangentX,
		const FVector & tangentY,
		const FVector & tangentZ
	) {
		FDynamicMeshVertex dmVert;
		dmVert.Position = vertex.Position;
		dmVert.Color = vertex.VertexColor;
		dmVert.SetTangents(tangentX, tangentY, tangentZ);
		int32 vindex = VertexBuffer.Vertices.Add(dmVert);
		IndexBuffer.Indices.Add(vindex);
	}
};

/** Scene proxy */
class FGeneratedMeshSceneProxy : public FPrimitiveSceneProxy {
private:
	// Maps mesh grouping to materials
	TMap<uint32, FMeshGroup> MeshGroups;

	FMaterialRelevance MaterialRelevance;

public:
	FGeneratedMeshSceneProxy(UGeneratedMeshComponent* Component)
		: FPrimitiveSceneProxy(Component),
		MaterialRelevance(Component->GetMaterialRelevance())
	{
		for (auto it : Component->MeshTriangles) {
			MeshGroups.Add(it.Key, FMeshGroup());
			FMeshGroup * const meshGroup = MeshGroups.Find(it.Key);
			for (auto it2 = it.Value.CreateConstIterator(); it2; ++it2) {
				const FMeshTriangle & Tri = *it2;

				const FVector Edge01 = (Tri.Vertex1.Position - Tri.Vertex0.Position);
				const FVector Edge02 = (Tri.Vertex2.Position - Tri.Vertex0.Position);

				const FVector tangentX = Edge01.SafeNormal();
				const FVector tangentZ = (Edge02 ^ Edge01).SafeNormal();
				const FVector tangentY = (tangentX ^ tangentZ).SafeNormal();

				meshGroup->BuildVertex(Tri.Vertex0, tangentX, tangentY, tangentZ);
				meshGroup->BuildVertex(Tri.Vertex1, tangentX, tangentY, tangentZ);
				meshGroup->BuildVertex(Tri.Vertex2, tangentX, tangentY, tangentZ);
			}

			meshGroup->Material = Component->GetMaterial(it.Key);
			meshGroup->InitializeResources();
		}
	}

	virtual ~FGeneratedMeshSceneProxy() {
		for (auto it = MeshGroups.CreateIterator(); it; ++it)
			it->Value.ReleaseResources();
	}

	virtual void DrawDynamicElements(FPrimitiveDrawInterface * PDI, const FSceneView * View) {
		QUICK_SCOPE_CYCLE_COUNTER(STAT_GeneratedMeshSceneProxy_DrawDynamicElements);

		const bool bWireframe = View->Family->EngineShowFlags.Wireframe;

		FColoredMaterialRenderProxy WireframeMaterialInstance(
			WITH_EDITOR ? GEngine->WireframeMaterial->GetRenderProxy(IsSelected()) : NULL,
			//GEngine->WireframeMaterial->GetRenderProxy(IsSelected()),
			FLinearColor(0, 0.5f, 1.f)
		);

		FMaterialRenderProxy * MaterialProxy = &WireframeMaterialInstance;

		FMeshBatch Mesh;
		FMeshBatchElement & BatchElement = Mesh.Elements[0];

		// For-range loops don't seem to work here, I get an `FRenderResource was
		// deleted without being released first!` error
		for (auto it = MeshGroups.CreateConstIterator(); it; ++it) {
			if (!bWireframe)
				MaterialProxy = it->Value.Material->GetRenderProxy(IsSelected());

			// Draw the mesh.
			BatchElement.IndexBuffer = &it->Value.IndexBuffer;
			Mesh.bWireframe = bWireframe;
			Mesh.VertexFactory = &it->Value.VertexFactory;
			Mesh.MaterialRenderProxy = MaterialProxy;
			BatchElement.PrimitiveUniformBuffer = CreatePrimitiveUniformBufferImmediate(
				GetLocalToWorld(), GetBounds(), GetLocalBounds(), true);
			BatchElement.FirstIndex = 0;
			BatchElement.NumPrimitives = it->Value.IndexBuffer.Indices.Num() / 3;
			BatchElement.MinVertexIndex = 0;
			BatchElement.MaxVertexIndex = it->Value.VertexBuffer.Vertices.Num() - 1;
			Mesh.ReverseCulling = IsLocalToWorldDeterminantNegative();
			Mesh.Type = PT_TriangleList;
			Mesh.DepthPriorityGroup = SDPG_World;
			PDI->DrawMesh(Mesh);
		}
	}

	virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) {
		FPrimitiveViewRelevance Result;
		Result.bDrawRelevance = IsShown(View);
		Result.bShadowRelevance = IsShadowCast(View);
		Result.bDynamicRelevance = true;
		MaterialRelevance.SetPrimitiveViewRelevance(Result);
		return Result;
	}

	virtual bool CanBeOccluded() const OVERRIDE {
		return !MaterialRelevance.bDisableDepthTest;
	}

	virtual uint32 GetMemoryFootprint(void) const {
		return sizeof(*this) + GetAllocatedSize();
	}

	uint32 GetAllocatedSize(void) const {
		return FPrimitiveSceneProxy::GetAllocatedSize();
	}
};



/////////////////////////////////////////////////////////////////////////////
// GeneratedMeshComponent Implementation
/////////////////////////////////////////////////////////////////////////////



UGeneratedMeshComponent::UGeneratedMeshComponent(
	const FPostConstructInitializeProperties & PCIP
	) : Super(PCIP), MeshTriangleCount(0) {
	PrimaryComponentTick.bCanEverTick = false;
}

bool UGeneratedMeshComponent::SetGeneratedMeshTriangles(
	const TArray<FMeshTriangle> & triangles
) {
	MeshTriangleCount = triangles.Num();

	MeshTriangles.Empty();
	MeshMaterials.Empty();

	for (auto it = triangles.CreateConstIterator(); it; ++it) {
		UMaterialInterface * mat = NULL;
		// If all three materials are the same, no blending needs to be done
		if (it->Vertex0.Material == it->Vertex1.Material &&
			it->Vertex0.Material == it->Vertex2.Material) {

			mat = it->Vertex0.Material;
		} else {
			// TODO(tlei): implement blending
		}

		if (mat == NULL)
			mat = UMaterial::GetDefaultMaterial(MD_Surface);

		// Search for index of material
		int32 index = MeshMaterials.IndexOfByKey(mat);
		if (index == INDEX_NONE)
			index = MeshMaterials.Add(mat);

		FMeshTriangle tri(*it);
		tri.MaterialIndex = index;

		// Get array of triangles from map
		if (!MeshTriangles.Contains(index))
			MeshTriangles.Add(index, TArray<FMeshTriangle>());

		MeshTriangles.Find(index)->Add(tri);
	}

#if WITH_EDITOR
	// This is required for the first time after creation
	if (ModelBodySetup)
		ModelBodySetup->InvalidatePhysicsData();
#endif

	UpdateCollision();

	// Need to recreate scene proxy to send it over
	MarkRenderStateDirty();

	return true;
}

void UGeneratedMeshComponent::ClearMeshTriangles() {
	MeshTriangles.Empty();
	MeshMaterials.Empty();
	MeshTriangleCount = 0;

#if WITH_EDITOR
	// This is required for the first time after creation
	if (ModelBodySetup)
		ModelBodySetup->InvalidatePhysicsData();
#endif

	UpdateCollision();

	// Need to recreate scene proxy to send it over
	MarkRenderStateDirty();
}


FPrimitiveSceneProxy* UGeneratedMeshComponent::CreateSceneProxy() {
	FPrimitiveSceneProxy* Proxy = NULL;
	if (MeshTriangleCount > 0)
		Proxy = new FGeneratedMeshSceneProxy(this);
	return Proxy;
}

int32 UGeneratedMeshComponent::GetNumMaterials() const {
	return MeshMaterials.Num();
}

UMaterialInterface * UGeneratedMeshComponent::GetMaterial(int32 ElementIndex) const {
	return MeshMaterials[ElementIndex];
}


FBoxSphereBounds UGeneratedMeshComponent::CalcBounds(const FTransform & LocalToWorld) const {
	FBoxSphereBounds NewBounds;
	NewBounds.Origin = FVector::ZeroVector;
	NewBounds.BoxExtent = FVector(HALF_WORLD_MAX, HALF_WORLD_MAX, HALF_WORLD_MAX);
	NewBounds.SphereRadius = FMath::Sqrt(3.0f * FMath::Square(HALF_WORLD_MAX));
	return NewBounds;
}


bool UGeneratedMeshComponent::GetPhysicsTriMeshData(
	struct FTriMeshCollisionData* CollisionData,
	bool InUseAllTriData
) {
	FTriIndices Triangle;

	for (auto it : MeshTriangles) {
		uint32 i = 0;
		for (auto it2 = it.Value.CreateConstIterator(); it2; ++it2, ++i) {
			Triangle.v0 = CollisionData->Vertices.Add(it2->Vertex0.Position);
			Triangle.v1 = CollisionData->Vertices.Add(it2->Vertex1.Position);
			Triangle.v2 = CollisionData->Vertices.Add(it2->Vertex2.Position);

			CollisionData->Indices.Add(Triangle);
			CollisionData->MaterialIndices.Add(i);
		}
	}

	CollisionData->bFlipNormals = true;

	return true;
}

bool UGeneratedMeshComponent::ContainsPhysicsTriMeshData(bool InUseAllTriData) const {
	return MeshTriangleCount > 0;
}

void UGeneratedMeshComponent::UpdateBodySetup() {
	if (ModelBodySetup == NULL)	{
		ModelBodySetup = ConstructObject<UBodySetup>(UBodySetup::StaticClass(), this);
		ModelBodySetup->CollisionTraceFlag = CTF_UseComplexAsSimple;
		ModelBodySetup->bMeshCollideAll = true;
	}
}

void UGeneratedMeshComponent::UpdateCollision() {
	if (bPhysicsStateCreated) {
		DestroyPhysicsState();
		UpdateBodySetup();
		CreatePhysicsState();

#if WITH_EDITOR
		ModelBodySetup->InvalidatePhysicsData();
#endif
		ModelBodySetup->CreatePhysicsMeshes();
	}
}

UBodySetup* UGeneratedMeshComponent::GetBodySetup() {
	UpdateBodySetup();
	return ModelBodySetup;
}
