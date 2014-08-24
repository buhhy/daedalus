#include <Daedalus.h>
#include "ItemCursor.h"

#include <Controllers/DDGameState.h>
#include <Utilities/Algebra/Algebra3D.h>
#include <Utilities/UnrealBridge.h>

using namespace items;
using namespace utils;

AItemCursor::AItemCursor(const class FPostConstructInitializeProperties & PCIP) :
	Super(PCIP), bIsHidden(false)
{
	SetActorEnableCollision(false);
	SetHidden(true);
}

Vector3D<> AItemCursor::GetOffsetVector() const {
	const auto & ipos = this->ItemData->Position;
	const auto & ppos = PlayerPosition;
	const auto result =
		this->TerrainParams->ToRealCoordSpace(ipos.ChunkOffset - ppos.ChunkOffset) +
			this->TerrainParams->ToRealCoordSpace(ipos.InnerOffset - ppos.InnerOffset);
	return result;
}

void AItemCursor::ApplyTransform() {
	const auto transform = this->ItemData->GetRotationMatrix();
	Basis3D basis = GetBasisFrom(transform);
	auto fRotMat = FRotationMatrix::MakeFromXZ(ToFVector(basis.XVector), ToFVector(basis.ZVector));

	const auto trans = this->GetOffsetVector() +
		GetTranslationVectorFrom(transform) * this->TerrainParams->ChunkGridUnitSize;
	// The player character naturally has a Z-rotation for looking around, we need to rotate the
	// item location and rotation by the inverse of that Z-rotation to ensure correct placement.
	const auto invPlayerRot = PlayerRotation.Inverse();
	SetRelativeTransform(
		invPlayerRot.TransformVector(ToFVector(trans)),
		(invPlayerRot * fRotMat).Rotator());
}

void AItemCursor::InvalidateCursor() {
	SetHidden(true);
	ItemData = NULL;
}

void AItemCursor::SetPlayerTransform(const Point3D & position, const FMatrix & rotation) {
	PlayerPosition = this->TerrainParams->ToGridCoordSpace(position);
	PlayerRotation = rotation;
}

void AItemCursor::SetHidden(const bool isHidden) {
	if (bIsHidden != isHidden) {
		bIsHidden = isHidden;
		SetActorHiddenInGame(bIsHidden);
	}
}
