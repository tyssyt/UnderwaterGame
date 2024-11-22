// Fill out your copyright notice in the Description page of Project Settings.

#include "MapTokenUI.h"

#include "MapGridUI.h"
#include "UI.h"

FReply UMapTokenUI::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) {
    if (!InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
        return FReply::Unhandled();

    const auto mapUI = UX::GetParentWidget<UMapGridUI>(this);
    if (!mapUI)
        return FReply::Unhandled();

    mapUI->Clicked(this);
    return FReply::Handled();
}

void UMapTokenUI::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) {
    Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
    Highlight(true);
}

void UMapTokenUI::NativeOnMouseLeave(const FPointerEvent& InMouseEvent) {
    Super::NativeOnMouseLeave(InMouseEvent);

    const auto mapUI = UX::GetParentWidget<UMapGridUI>(this);
    if (mapUI && mapUI->IsSelected(this))
        return;

    Highlight(false);
}

void UMapTokenUI::Highlight(bool highlighted) {
    if (highlighted)
        Image->SetBrushTintColor(FSlateColor(FLinearColor(.0f, 0.35f, .0f)));
    else
        Image->SetBrushTintColor(FSlateColor(FLinearColor(1.f, 1.f, 1.f)));
}
