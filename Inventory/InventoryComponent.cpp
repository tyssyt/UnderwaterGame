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

FInventorySlot* UInventoryComponent::GetInput(const UResource* resource) {
    for (auto& input : GetInputs())
        if (input.Resource == resource)
            return &input;
    return nullptr;
}

FInventorySlot* UInventoryComponent::GetOutput(const UResource* resource) {
    for (auto& output : GetOutputs())
        if (output.Resource == resource)
            return &output;
    return nullptr;
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

void UInventoryComponent::AddToSelectedUI(TArray<UBuildingSelectedUIComponent*>& components) {
    components.Add(NewObject<UInventoryComponentUI>(this)->Init(this));
}

UInventoryComponentUI* UInventoryComponentUI::Init(UInventoryComponent* inventoryComponent) {
    InventoryComponent = inventoryComponent;
    return this;
}

void UInventoryComponentUI::CreateUI(UBuildingSelectedUI* selectedUI) {
    UI = CreateWidget<UInventoryUI>(selectedUI, The::BPHolder(this)->InventoryUIClass)->Init(InventoryComponent);
    const auto slot = selectedUI->Content->AddChildToVerticalBox(UI);
    slot->SetHorizontalAlignment(HAlign_Center);
}

void UInventoryComponentUI::Tick(UBuildingSelectedUI* selectedUI) {
    UI->Tick();
}
