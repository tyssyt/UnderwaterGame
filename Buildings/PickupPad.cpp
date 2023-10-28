// Fill out your copyright notice in the Description page of Project Settings.

#include "PickupPad.h"

#include "The.h"
#include "XD/Construction/ConstructionManager.h"

APickupPad::APickupPad() {
    const static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshFinder(TEXT("/Game/Assets/Meshes/Pickup_Pad"));
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMesh0"));
    Mesh->SetStaticMesh(MeshFinder.Object);
    SetRootComponent(Mesh);

    Inventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));
    Inventory->GetInputs().Emplace(50000);
    Inventory->GetInputs().Emplace(50000);
    Inventory->GetInputs().Emplace(50000);
    Inventory->GetInputs().Emplace(50000);
}

void APickupPad::OnConstructionComplete(UConstructionOptions* options) {
    Super::OnConstructionComplete(options);
    The::ConstructionManager(this)->AddPickupPad(this);
}


void UPickupPadUI::Tick() {
    if (PickupPad) {
        const FInventorySlot* inv1 = &PickupPad->Inventory->GetInputs()[0];
        InventorySlot_1->Set(inv1);
        const FInventorySlot* inv2 = &PickupPad->Inventory->GetInputs()[1];
        InventorySlot_2->Set(inv2);
        const FInventorySlot* inv3 = &PickupPad->Inventory->GetInputs()[2];
        InventorySlot_3->Set(inv3);
        const FInventorySlot* inv4 = &PickupPad->Inventory->GetInputs()[3];
        InventorySlot_4->Set(inv4);
    }
}
