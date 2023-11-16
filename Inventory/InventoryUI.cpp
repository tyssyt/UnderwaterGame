// Fill out your copyright notice in the Description page of Project Settings.

#include "InventoryUI.h"

#include "InventoryComponent.h"

UInventoryUI* UInventoryUI::Init(UInventoryComponent* inventory) {
    Inventory = inventory;
    Inputs->ClearChildren();
    for (const auto& input : inventory->GetInputs())
        Inputs->AddChildToWrapBox(CreateWidget<UInventorySlotUI>(WidgetTree, InventorySlotUIClass)->Set(&input));
    Outputs->ClearChildren();
    for (const auto& output : inventory->GetOutputs())
        Outputs->AddChildToWrapBox(CreateWidget<UInventorySlotUI>(WidgetTree, InventorySlotUIClass)->Set(&output));

    if (Inputs->GetChildrenCount() == 0 || Outputs->GetChildrenCount() == 0)
        Arrows->SetVisibility(ESlateVisibility::Collapsed);
    
    return this;
}

void UInventoryUI::Tick() const {
    const auto& inputs = Inventory->GetInputs();
    for (int idx = 0; idx < inputs.Num(); idx++)
        Cast<UInventorySlotUI>(Inputs->GetChildAt(idx))->Set(&inputs[idx]);

    const auto& outputs = Inventory->GetOutputs();
    for (int idx = 0; idx < outputs.Num(); idx++)
        Cast<UInventorySlotUI>(Outputs->GetChildAt(idx))->Set(&outputs[idx]);
}
