// Fill out your copyright notice in the Description page of Project Settings.

#include "HotbarSlotSubmenu.h"

#include "HotbarDock.h"
#include "UI.h"


UTexture2D* GetTexture2D(const UImage* Image) {
    return Cast<UTexture2D>(Image->GetBrush().GetResourceObject());
}

UHotbarSlotSubmenu* UHotbarSlotSubmenu::Init(UHotbar* submenu) {
    if (const auto slot = submenu->GetSlot(1))
        Image_1->SetBrushFromTexture(GetTexture2D(slot->GetImage()));
    if (const auto slot = submenu->GetSlot(2))
        Image_2->SetBrushFromTexture(GetTexture2D(slot->GetImage()));
    if (const auto slot = submenu->GetSlot(3)) 
        Image_3->SetBrushFromTexture(GetTexture2D(slot->GetImage()));
    if (const auto slot = submenu->GetSlot(4))
        Image_4->SetBrushFromTexture(GetTexture2D(slot->GetImage()));

    Submenu = submenu;
    return this;
}

void UHotbarSlotSubmenu::Activate() {
    GetDock()->OpenSubmenu(Submenu, GetHotbar());

    const auto button = UX::GetParentWidget<UButton>(this);
    button->SetBackgroundColor(FLinearColor::Yellow);
}

UImage* UHotbarSlotSubmenu::GetImage() {
    return Image_1;
}
