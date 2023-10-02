// Fill out your copyright notice in the Description page of Project Settings.

#include "Hotbar.h"

#include "HotbarDock.h"
#include "HotbarSlot.h"
#include "Components/NamedSlot.h"
#include "XD/Utils.h"

UHotbar* UHotbar::Init(
    UHotbarSlot* slot1,
    UHotbarSlot* slot2,
    UHotbarSlot* slot3,
    UHotbarSlot* slot4,
    UHotbarSlot* slot5,
    UHotbarSlot* slot6,
    UHotbarSlot* slot7,
    UHotbarSlot* slot8,
    UHotbarSlot* slot9,
    UHotbarSlot* slot0
) {
    if (slot1)
        NamedSlot_1->SetContent(slot1);
    if (slot2)
        NamedSlot_2->SetContent(slot2);
    if (slot3)
        NamedSlot_3->SetContent(slot3);
    if (slot4)
        NamedSlot_4->SetContent(slot4);
    if (slot5)
        NamedSlot_5->SetContent(slot5);
    if (slot6)
        NamedSlot_6->SetContent(slot6);
    if (slot7)
        NamedSlot_7->SetContent(slot7);
    if (slot8)
        NamedSlot_8->SetContent(slot8);
    if (slot9)
        NamedSlot_9->SetContent(slot9);
    if (slot0)
        NamedSlot_0->SetContent(slot0);

    return this;
}

UHotbarDock* UHotbar::GetDock() {
    return UI::GetParentWidget<UHotbarDock>(this);
}

UHotbarSlot* UHotbar::GetSlot(const int i) const {
    switch (i) {
    case 1:
        return Cast<UHotbarSlot>(NamedSlot_1->GetContent());
    case 2:
        return Cast<UHotbarSlot>(NamedSlot_2->GetContent());
    case 3:
        return Cast<UHotbarSlot>(NamedSlot_3->GetContent());
    case 4:
        return Cast<UHotbarSlot>(NamedSlot_4->GetContent());
    case 5:
        return Cast<UHotbarSlot>(NamedSlot_5->GetContent());
    case 6:
        return Cast<UHotbarSlot>(NamedSlot_6->GetContent());
    case 7:
        return Cast<UHotbarSlot>(NamedSlot_7->GetContent());
    case 8:
        return Cast<UHotbarSlot>(NamedSlot_8->GetContent());
    case 9:
        return Cast<UHotbarSlot>(NamedSlot_9->GetContent());
    case 0:
        return Cast<UHotbarSlot>(NamedSlot_0->GetContent());
    default:
        return nullptr;
    }
}

void UHotbar::Reset() const {
    Button_1->SetBackgroundColor(FLinearColor::White);
    Button_2->SetBackgroundColor(FLinearColor::White);
    Button_3->SetBackgroundColor(FLinearColor::White);
    Button_4->SetBackgroundColor(FLinearColor::White);
    Button_5->SetBackgroundColor(FLinearColor::White);
    Button_6->SetBackgroundColor(FLinearColor::White);
    Button_7->SetBackgroundColor(FLinearColor::White);
    Button_8->SetBackgroundColor(FLinearColor::White);
    Button_9->SetBackgroundColor(FLinearColor::White);
    Button_0->SetBackgroundColor(FLinearColor::White);
}
