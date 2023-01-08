// Fill out your copyright notice in the Description page of Project Settings.

#include "InventoryComponent.h"

UInventoryComponent::UInventoryComponent() {
    PrimaryComponentTick.bCanEverTick = false;
}

TArray<FInventorySlot>& UInventoryComponent::GetInputs() {
    return Inputs;
}

TArray<FInventorySlot>& UInventoryComponent::GetOutputs() {
    return IsBuffer ? Inputs : Outputs; // Performance: this can be called many times per frame and never changes after init, should be done without branching!
}
