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
	const auto & ppos = playerPosition;
	const auto result =
		this->TerrainParams->ToRealCoordSpace(ipos.ChunkOffset - ppos.ChunkOffset) +
			this->TerrainParams->ToRealCoordSpace(ipos.InnerOffset - ppos.InnerOffset);
	return result;
}

void AItemCursor::ApplyTransform() {
	AssertInitialized();
	if (lastPosition != ItemData->getPosition() ||
			lastRotation != ItemData->getRotation() ||
			lastPlayerPosition != playerPosition ||
			lastPlayerRotation != playerRotation)
	{
		lastRotation = ItemData->getRotation();
		lastPosition = ItemData->getPosition();
		lastPlayerPosition = playerPosition;
		lastPlayerRotation = playerRotation;

		const auto transform = this->ItemData->GetRotationMatrix();
		Basis3D basis = GetBasisFrom(transform);
		auto fRotMat = FRotationMatrix::MakeFromXZ(ToFVector(basis.XVector), ToFVector(basis.ZVector));

		const auto trans = this->GetOffsetVector() +
			GetTranslationVectorFrom(transform) * this->TerrainParams->ChunkGridUnitSize;
		// The player character naturally has a Z-rotation for looking around, we need to rotate the
		// item location and rotation by the inverse of that Z-rotation to ensure correct placement.
		const auto invPlayerRot = playerRotation.Inverse();
		SetRelativeTransform(
			invPlayerRot.TransformVector(ToFVector(trans)),
			(invPlayerRot * fRotMat).Rotator());
	}
}

void AItemCursor::InvalidateCursor() {
	SetHidden(true);
	ItemData = NULL;
}

void AItemCursor::SetPlayerTransform(const Point3D & position, const FMatrix & rotation) {
	playerPosition = this->TerrainParams->ToGridCoordSpace(position);
	playerRotation = rotation;
}

void AItemCursor::SetHidden(const bool isHidden) {
	if (bIsHidden != isHidden) {
		bIsHidden = isHidden;
		SetActorHiddenInGame(bIsHidden);
	}
}
