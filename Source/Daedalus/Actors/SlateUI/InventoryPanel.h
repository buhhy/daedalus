#pragma once

#include "GameFramework/HUD.h"

class APlayerHUD;

class SInventoryPanel : public SCompoundWidget {
	SLATE_BEGIN_ARGS(SInventoryPanel) {}
	SLATE_ARGUMENT(TWeakObjectPtr<APlayerHUD>, OwningHUD);
	SLATE_END_ARGS()

private:
	TWeakObjectPtr<APlayerHUD> OwningHUD;

public:
	void Construct(const FArguments & inArgs);

};