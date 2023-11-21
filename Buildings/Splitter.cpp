// Fill out your copyright notice in the Description page of Project Settings.


#include "Splitter.h"

#include "Conveyor.h"

constexpr int COMBINED_SLOT_SIZE = 150;
constexpr int SPLIT_SLOT_SIZE = 100;
constexpr double SPLIT_SLOT_SIZE_D = 100.;

void AJunction::InternalDisconnect(bool isSplitter, UInventoryComponent* InventoryComponent, AConveyor* conveyor) {
    auto& inventory = isSplitter ? InventoryComponent->GetOutputs() : InventoryComponent->GetInputs();

    const int idx = (isSplitter ? conveyor->SourceInv : conveyor->TargetInv) - inventory.GetData();
    check(idx >= 0 && idx < inventory.Num());

    // we don't waste
    if (isSplitter)
        InventoryComponent->GetInputs()[0].Current += inventory[idx].Current;
    else
        InventoryComponent->GetOutputs()[0].Current += inventory[idx].Current;

    // swap with last
    if (idx != Connections-1) {
        inventory[idx] = inventory[Connections-1];
        if (isSplitter)
            inventory[idx].Conveyor->SourceInv = &inventory[idx];
        else
            inventory[idx].Conveyor->TargetInv = &inventory[idx];
    }

    // reset connection
    auto& toClear = inventory[Connections-1];
    toClear.Current = 0;
    toClear.Conveyor = nullptr;
    Connections--;

    if (isSplitter)
        conveyor->SourceInv = nullptr;
    else
        conveyor->TargetInv = nullptr;
}

ASplitter::ASplitter() {
    PrimaryActorTick.bCanEverTick = true;
    
    Mesh = CreateDefaultSubobject<UConveyorNode>(TEXT("BlockMesh"));
    SetRootComponent(Mesh);
    
    Inventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));
    Inventory->GetInputs().Emplace(COMBINED_SLOT_SIZE);
    Inventory->GetOutputs().Emplace(SPLIT_SLOT_SIZE);
    Inventory->GetOutputs().Emplace(SPLIT_SLOT_SIZE);
    Inventory->GetOutputs().Emplace(SPLIT_SLOT_SIZE);
    Inventory->GetOutputs().Emplace(SPLIT_SLOT_SIZE);
}

void ASplitter::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);

    FInventorySlot* input = &Inventory->GetInputs()[0];
    
    const int amount = input->PullFrom(COMBINED_SLOT_SIZE);
    if (amount == 0)
        return;
    
    TArray<FInventorySlot>* outputs = &Inventory->GetOutputs();

    const int amountPerConnection = amount / Connections;
    int overflow = amount % Connections;
    
    for (int i = 0; i < Connections; ++i)
        overflow += amountPerConnection - (*outputs)[i].PushInto(amountPerConnection);

    input->Current += overflow;
}

void ASplitter::Disconnect(AConveyor* conveyor) {
    InternalDisconnect(true, Inventory, conveyor);
}

AMerger::AMerger() {
    PrimaryActorTick.bCanEverTick = true;
    
    Mesh = CreateDefaultSubobject<UConveyorNode>(TEXT("BlockMesh"));
    SetRootComponent(Mesh);
    
    Inventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));
    Inventory->GetInputs().Emplace(SPLIT_SLOT_SIZE);
    Inventory->GetInputs().Emplace(SPLIT_SLOT_SIZE);
    Inventory->GetInputs().Emplace(SPLIT_SLOT_SIZE);
    Inventory->GetInputs().Emplace(SPLIT_SLOT_SIZE);
    Inventory->GetOutputs().Emplace(COMBINED_SLOT_SIZE);
}

void AMerger::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);

    FInventorySlot* output = &Inventory->GetOutputs()[0];
    if (output->Current >= SPLIT_SLOT_SIZE)
        return;
    
    
    TArray<FInventorySlot>* inputs = &Inventory->GetInputs();
    int totalInput = 0;
    for (int i = 0; i < Connections; ++i)
        totalInput += (*inputs)[i].Current;

    const float factor = FMath::Clamp((SPLIT_SLOT_SIZE_D - output->Current) / totalInput, 0., 1.);

    // TODO optimization
    // Idea 1: quite often, factor will be 1 so we can do a branch without float math and float/int conversion, but that adds a branch...
    // Idea 2: I believe there could be a way to do the math with only ints

    int pulled = 0;
    for (int i = 0; i < Connections; ++i) {
        FInventorySlot* input = &(*inputs)[i];
        pulled += input->PullFrom(FMath::CeilToInt(factor * input->Current));
    }

    output->Current += pulled;
}

void AMerger::Disconnect(AConveyor* conveyor) {
    InternalDisconnect(false, Inventory, conveyor);
}
