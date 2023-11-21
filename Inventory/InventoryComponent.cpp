// Fill out your copyright notice in the Description page of Project Settings.

#include "InventoryComponent.h"

#include "The.h"
#include "Components/VerticalBoxSlot.h"
#include "XD/BlueprintHolder.h"
#include "XD/Recipes/Recipe.h"

UInventoryComponent::UInventoryComponent() {
    PrimaryComponentTick.bCanEverTick = false;
}

void UInventoryComponent::SetBuffer() {
    check(Inputs.Num() == 1);
    check(Outputs.Num() == 0);
    bIsBuffer = true;
    Outputs.Emplace(0); // only for storing the outgoing connection
}

TArray<FInventorySlot*> UInventoryComponent::GetUnconnected(bool isInput) {
    TArray<FInventorySlot*> unconnected;
    if (bIsBuffer && !isInput) {
        check(Inputs.Num() == Outputs.Num());
        for (int idx = 0; idx < Inputs.Num(); idx++)
            if (Outputs[idx].Conveyor == nullptr)
                unconnected.Add(&Inputs[idx]);
    } else {
        for (auto& slot : isInput ? Inputs : Outputs)
            if (slot.Conveyor == nullptr)
                unconnected.Add(&slot);
    }
    return MoveTemp(unconnected);
}

void UInventoryComponent::SetConveyor(bool isInput, FInventorySlot* slot, AConveyor* conveyor) {
    if (bIsBuffer && !isInput) {
        const int idx = slot - Inputs.GetData();
        check(idx >= 0 && idx < Inputs.Num());
        Outputs[idx].Conveyor = conveyor;
    } else
        slot->Conveyor = conveyor;
}

void UInventoryComponent::SetRecipe(URecipe* recipe) {
    check(!bIsBuffer);

    Inputs.Empty();
    for (const auto& ingredient : recipe->Ingredients)
        Inputs.Emplace(ingredient.amount*2, ingredient.resource);

    Outputs.Empty();
    for (const auto& result : recipe->Results)
        Outputs.Emplace(result.amount*2, result.resource);
}

void UInventoryComponent::AddToSelectedUI(UBuildingSelectedUI* selectedUI) {
    if (!AddToUI)
        return;
    const auto ui = CreateWidget<UInventoryUI>(selectedUI, The::BPHolder(this)->InventoryUIClass)->Init(this);
    const auto slot = selectedUI->Content->AddChildToVerticalBox(ui);
    slot->SetHorizontalAlignment(HAlign_Center);
    selectedUI->Storage->Data.Add(StaticClass(), NewObject<UInventoryComponentSelectedData>()->Init(ui));
}

void UInventoryComponent::UpdateSelectedUI(UBuildingSelectedUI* selectedUI) {
    if (!AddToUI)
        return;
    const auto data = selectedUI->Storage->Get<UInventoryComponentSelectedData>(StaticClass());
    check(data);
    data->UI->Tick();
}

UInventoryComponentSelectedData* UInventoryComponentSelectedData::Init(UInventoryUI* ui) {
    UI = ui;
    return this;
}
