#pragma once

#include "CommonUILibrary.h"
#include "Blueprint/WidgetTree.h"
#include "Components/SizeBox.h"

namespace UI {
    template <class T>
    static T* GetParentWidget(UWidget* Widget) {
        return Cast<T>(UCommonUILibrary::FindParentWidgetOfType(Widget, T::StaticClass()));
    }
    template <class T>
    static T* GetParentWidget(const UWidget* Widget) {
        return Cast<T>(UCommonUILibrary::FindParentWidgetOfType(const_cast<UWidget*>(Widget), T::StaticClass()));
    }
    // TODO unreal build tool decided not to include the UI.cpp in SOME of the modules, so into the header we go (c++ is lovely)
    static USizeBox* Sized(const UUserWidget* outer, UWidget* inner, float width, float height) {
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

    static FSlateFontInfo WithSize(const FSlateFontInfo& base, const int size) {
        FSlateFontInfo newFont = base;
        newFont.Size = size;
        return MoveTemp(newFont);
    }
}
