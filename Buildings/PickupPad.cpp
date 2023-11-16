// Fill out your copyright notice in the Description page of Project Settings.

#include "PickupPad.h"

#include "The.h"
#include "XD/Construction/ConstructionManager.h"

APickupPad::APickupPad() {
    const static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshFinder(TEXT("/Game/Assets/Meshes/Pickup_Pad"));
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMesh0"));
    Mesh->SetStaticMesh(MeshFinder.Object);
    Mesh->SetRenderCustomDepth(true);
    SetRootComponent(Mesh);

    Inventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));
    Inventory->GetInputs().Emplace(40000);
    Inventory->GetInputs().Emplace(40000);
    Inventory->GetInputs().Emplace(40000);
    Inventory->GetInputs().Emplace(40000);
    Inventory->AddToUI = true;
}

void APickupPad::OnConstructionComplete(UConstructionOptions* options) {
    Super::OnConstructionComplete(options);
    The::ConstructionManager(this)->AddPickupPad(this);
}
