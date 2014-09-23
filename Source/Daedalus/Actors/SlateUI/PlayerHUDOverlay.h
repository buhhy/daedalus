#pragma once

#include <GameFramework/HUD.h>
#include <Models/Fauna/CharacterData.h>

#include <memory>

class APlayerHUD;

class SPlayerHUDOverlay : public SCompoundWidget {
	SLATE_BEGIN_ARGS(SPlayerHUDOverlay) {}
	SLATE_ARGUMENT(TWeakObjectPtr<APlayerHUD>, OwningHUD);
	SLATE_END_ARGS()

private:
	TWeakObjectPtr<APlayerHUD> OwningHUD;

	std::weak_ptr<fauna::CharacterData> characterData;

public:
	void Construct(const FArguments & inArgs);

	void setCharacterData(const std::weak_ptr<fauna::CharacterData> & charData) {
		this->characterData = charData;
	}
};