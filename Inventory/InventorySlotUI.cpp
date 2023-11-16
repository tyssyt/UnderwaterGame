// Fill out your copyright notice in the Description page of Project Settings.

#include "InventorySlotUI.h"

UInventorySlotUI* UInventorySlotUI::Set(const FInventorySlot* inventorySlot) {
    if (!inventorySlot || !inventorySlot->Resource) {
        Clear();
    } else {
        Set(inventorySlot->Current, inventorySlot->Max, inventorySlot->Resource);
    }
    return this;
}

UInventorySlotUI* UInventorySlotUI::Set(int current, int max, UResource* resource) {
    Current->SetText(FText::AsNumber(current, &FNumberFormattingOptions::DefaultNoGrouping()));
    Max->SetText(FText::AsNumber(max, &FNumberFormattingOptions::DefaultNoGrouping()));
    if (max > 0) 
        SetFillPercent(static_cast<float>(current) / max);
    else
        SetFillPercent(.0f);

    Resource->SetRenderOpacity(1.f);
    Resource->Init(resource);
    SetToolTipText(resource->Name);
    return this;
}

void UInventorySlotUI::Clear() {   
    Current->SetText(FText::FromString(TEXT("0")));
    Max->SetText(FText::FromString(TEXT("0")));
    SetFillPercent(.0f);

    Resource->SetRenderOpacity(0.f);
    SetToolTipText(FText::FromString(TEXT(""))); // TODO test if this works
}

void UInventorySlotUI::SetFillPercent(float fill) const {
    // TODO this can probably be optimized to use a single point of branching which could be faster...
    fill = fill * 4.f;

    if (fill < 1.f) {
        Fill_1->SetPercent(fill);
        Fill_2->SetPercent(0.f);
        Fill_3->SetPercent(0.f);
        Fill_4->SetPercent(0.f);
        return;
    }
    Fill_1->SetPercent(1.f);

    fill = fill - 1.f;
    if (fill < 1.f) {
        Fill_2->SetPercent(fill);
        Fill_3->SetPercent(0.f);
        Fill_4->SetPercent(0.f);
        return;
    }
    Fill_2->SetPercent(1.f);

    fill = fill - 1.f;
    if (fill < 1.f) {
        Fill_3->SetPercent(fill);
        Fill_4->SetPercent(0.f);
        return;
    }
    Fill_3->SetPercent(1.f);

    fill = fill - 1.f;
    Fill_4->SetPercent(fill);
}
