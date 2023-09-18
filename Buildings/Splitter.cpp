// Fill out your copyright notice in the Description page of Project Settings.


#include "Splitter.h"

#include "Conveyor.h"

ASplitter::ASplitter() {
    PrimaryActorTick.bCanEverTick = true;
    
    Mesh = CreateDefaultSubobject<UConveyorNode>(TEXT("BlockMesh"));
    SetRootComponent(Mesh);
    
    Inventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));
    Inventory->GetInputs().Emplace(150);
    Inventory->GetOutputs().Emplace(100);
    Inventory->GetOutputs().Emplace(100);
    Inventory->GetOutputs().Emplace(100);
    Inventory->GetOutputs().Emplace(100);
}

void ASplitter::BeginPlay() {
    Super::BeginPlay();
}

void ASplitter::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);

    FInventorySlot* input = &Inventory->GetInputs()[0];
    
    const int amount = input->PullFrom(150);
    if (amount == 0)
        return;
    
    TArray<FInventorySlot>* outputs = &Inventory->GetOutputs();

    const int amountPerConnection = amount / Connections;
    int overflow = amount % Connections;
    
    for (int i = 0; i < Connections; ++i) {
        overflow += amountPerConnection - (*outputs)[i].PushInto(amountPerConnection);
    }

    input->Current += overflow;
}

AMerger::AMerger() {
    PrimaryActorTick.bCanEverTick = true;
    
    Mesh = CreateDefaultSubobject<UConveyorNode>(TEXT("BlockMesh"));
    SetRootComponent(Mesh);
    
    Inventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));
    Inventory->GetInputs().Emplace(100);
    Inventory->GetInputs().Emplace(100);
    Inventory->GetInputs().Emplace(100);
    Inventory->GetInputs().Emplace(100);
    Inventory->GetOutputs().Emplace(150);
}

void AMerger::BeginPlay() {
    Super::BeginPlay();
}

void AMerger::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);

    FInventorySlot* output = &Inventory->GetOutputs()[0];
    if (output->Current >= 100)
        return;
    
    
    TArray<FInventorySlot>* inputs = &Inventory->GetInputs();
    int totalInput = 0;
    for (int i = 0; i < Connections; ++i)
        totalInput += (*inputs)[i].Current;
    
    float factor = FMath::Clamp((100. - output->Current) / totalInput, 0., 1.);

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
