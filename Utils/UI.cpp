#include "UI.h"

/*
#include "Blueprint/WidgetTree.h"

USizeBox* UI::Sized(const UUserWidget* outer, UWidget* inner, float width, float height) {
    if (const auto innerSizeBox = Cast<USizeBox>(inner)) {
        innerSizeBox->SetWidthOverride(width);
        innerSizeBox->SetHeightOverride(height);
        return innerSizeBox;
    }

    const auto sizeBox = outer->WidgetTree->ConstructWidget<USizeBox>();
    sizeBox->SetWidthOverride(width);
    sizeBox->SetHeightOverride(height);
    sizeBox->AddChild(inner);    
    return sizeBox;
}
*/
