#pragma once

#include "CommonUILibrary.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/SizeBox.h"

struct UX { // TODO could just be a namespace, but that breaks the build somehow -.- <3 unreal
    template <class T>
    static T* GetParentWidget(UWidget* Widget) {
        return Cast<T>(UCommonUILibrary::FindParentWidgetOfType(Widget, T::StaticClass()));
    }
    template <class T>
    static T* GetParentWidget(const UWidget* Widget) {
        return Cast<T>(UCommonUILibrary::FindParentWidgetOfType(const_cast<UWidget*>(Widget), T::StaticClass()));
    }
    static USizeBox* Sized(UWidgetTree* tree, UWidget* inner, float width, float height);
    static UBorder* WithBorder(UWidgetTree* tree, UWidget* inner, FLinearColor color);
    static FSlateFontInfo WithSize(const FSlateFontInfo& base, const int size);
    static UButton* CreateImageButton(UWidgetTree* tree, UTexture2D* texture, FVector2d size, bool border);
    static UBorder* CreateToken(UWidgetTree* tree, const FText& text);
};
