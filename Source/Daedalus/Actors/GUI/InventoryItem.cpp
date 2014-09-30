#include <Daedalus.h>
#include "InventoryItem.h"

#include <Actors/GUI/PlayerHUD.h>

void SInventorySlot::Construct(const FArguments & inArgs) {
	OwningHUD = inArgs._OwningHUD;
	ChildSlot
		.VAlign(VAlign_Fill)
		.HAlign(HAlign_Fill)
		[
			SNew(SOverlay)
			+ SOverlay::Slot()
			.VAlign(VAlign_Top)
			.HAlign(HAlign_Center)
			[
				SNew(STextBlock)
				.ShadowColorAndOpacity(FLinearColor::Black)
				.ColorAndOpacity(FLinearColor::White)
				.ShadowOffset(FIntPoint(-1, 1))
				.Font(FSlateFontInfo("Veranda", 16)) //don't think this actually changes the font
				.Text(FText::FromString("Hello, Slate!"))
			]
		];
}