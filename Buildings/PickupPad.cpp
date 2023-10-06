// Fill out your copyright notice in the Description page of Project Settings.

#include "PickupPad.h"

#include "XD/GameInstanceX.h"
#include "XD/Utils.h"


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

void APickupPad::BeginPlay() {
    Super::BeginPlay();
}

void APickupPad::OnConstructionComplete(FConstructionFlags flags) {
    Super::OnConstructionComplete(flags);
    The::ConstructionManager(this)->AddPickupPad(this);
}


void UPickupPadUI::Tick() {
    if (PickupPad) {
        FInventorySlot* inv1 = &PickupPad->Inventory->GetInputs()[0];
        InventorySlot_1->Set(inv1);
        FInventorySlot* inv2 = &PickupPad->Inventory->GetInputs()[1];
        InventorySlot_2->Set(inv2);
        FInventorySlot* inv3 = &PickupPad->Inventory->GetInputs()[2];
        InventorySlot_3->Set(inv3);
        FInventorySlot* inv4 = &PickupPad->Inventory->GetInputs()[3];
        InventorySlot_4->Set(inv4);
    }
}
