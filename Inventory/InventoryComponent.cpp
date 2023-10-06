// Fill out your copyright notice in the Description page of Project Settings.

#include "InventoryComponent.h"

#include "XD/Recipes/Recipe.h"

UInventoryComponent::UInventoryComponent() {
    PrimaryComponentTick.bCanEverTick = false;
}

void UInventoryComponent::SetBuffer() {
    check(Inputs.Num() == 1);
    check(Outputs.Num() == 0);
    IsBuffer = true;
    Outputs.Emplace(0); // only for storing the outgoing connection
}

TArray<FInventorySlot>& UInventoryComponent::GetInputs() {
    return Inputs;
}

TArray<FInventorySlot>& UInventoryComponent::GetOutputs() {
    return IsBuffer ? Inputs : Outputs; // Performance: this can be called many times per frame and never changes after init, should be done without branching!
}

void UInventoryComponent::SetRecipe(URecipe* recipe) {
    check(!IsBuffer);

    Inputs.Empty();
    for (const auto& ingredient : recipe->Ingredients)
        Inputs.Emplace(ingredient.amount*2, ingredient.resource);

    Outputs.Empty();
    for (const auto& result : recipe->Results)
        Outputs.Emplace(result.amount*2, result.resource);
}
