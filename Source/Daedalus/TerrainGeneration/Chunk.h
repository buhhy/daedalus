// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "GeneratedMeshComponent.h"
#include "Chunk.generated.h"

USTRUCT()
struct FDensityBlock {
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category = Data)
		TArray<uint32> DensityData;

	UPROPERTY(EditAnywhere, Category = Dimensions)
		uint32 Width;		// X

	UPROPERTY(EditAnywhere, Category = Dimensions)
		uint32 Depth;		// Y

	UPROPERTY(EditAnywhere, Category = Dimensions)
		uint32 Height;		// Z

	FDensityBlock() : Width(0), Depth(0), Height(0) {}

	void Init(uint32 Width, uint32 Depth, uint32 Height) {
		this->Width = Width;
		this->Height = Height;
		this->Depth = Depth;
		DensityData.InsertZeroed(0, Width * Height * Depth);
	}
	
	const uint32 & GetDensityAt(const uint32 & x, const uint32 & y, const uint32 & z) const {
		return DensityData[(x * Width + y) * Height + z];
	}

	void SetDensityAt(const uint32 & x, const uint32 & y, const uint32 & z, uint32 value) {
		DensityData[(x * Width + y) * Height + z] = value;
	}
};

/**
*
*/
UCLASS()
class AChunk: public AActor
{
	GENERATED_UCLASS_BODY()

	double GenerateRandomNumber(uint16 relativeX, uint16 relativeY, uint16 relativeZ);
	void RunDiamondSquare();
	void TestRender();
	void SetDefaultHeight(uint32 height);

public:
	UPROPERTY()
		TSubobjectPtr<UGeneratedMeshComponent> Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Offset)
		uint64 OffsetX;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Offset)
		uint64 OffsetY;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Offset)
		uint64 OffsetZ;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Offset)
		uint64 Seed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Data)
		FDensityBlock Densities;

	UFUNCTION(BlueprintCallable, Category = Initialization)
		void InitializeChunk(
			uint16 _chunkWidth, uint16 _chunkDepth, uint16 _chunkHeight,
			uint64 _offsetX, uint64 _offsetY, uint64 _offsetZ,
			uint64 _seed);
};
