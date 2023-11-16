#include "UI.h"


#include "Blueprint/WidgetTree.h"

USizeBox* UX::Sized(UWidgetTree* tree, UWidget* inner, float width, float height) {
    if (const auto innerSizeBox = Cast<USizeBox>(inner)) {
        innerSizeBox->SetWidthOverride(width);
        innerSizeBox->SetHeightOverride(height);
        return innerSizeBox;
    }

    const auto sizeBox = tree->ConstructWidget<USizeBox>();
    sizeBox->SetWidthOverride(width);
    sizeBox->SetHeightOverride(height);
    sizeBox->AddChild(inner);    
    return sizeBox;
}

FSlateFontInfo UX::WithSize(const FSlateFontInfo& base, const int size) {
    FSlateFontInfo newFont = base;
    newFont.Size = size;
    return MoveTemp(newFont);
}

UButton* UX::CreateImageButton(UWidgetTree* tree, UTexture2D* texture, FVector2d size, bool border) {
    const auto button = tree->ConstructWidget<UButton>();
    FButtonStyle style = button->GetStyle();
    style.Normal.SetResourceObject(texture);
    style.Normal.SetImageSize(size);
    style.Hovered.SetResourceObject(texture);
    style.Hovered.SetImageSize(size);
    style.Pressed.SetResourceObject(texture);
    style.Pressed.SetImageSize(size);
    style.Disabled.SetResourceObject(texture);
    style.Disabled.SetImageSize(size);
    if (!border) {
        style.Normal.DrawAs = ESlateBrushDrawType::Image;
        style.Hovered.DrawAs = ESlateBrushDrawType::Image;
        style.Pressed.DrawAs = ESlateBrushDrawType::Image;
        style.Disabled.DrawAs = ESlateBrushDrawType::Image;
    }
    button->SetStyle(style);
    return button;
}
