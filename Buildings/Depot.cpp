// Fill out your copyright notice in the Description page of Project Settings.

#include "Depot.h"
#include "Application/SlateApplicationBase.h"


ADepot::ADepot() {
    static ConstructorHelpers::FObjectFinderOptional<UStaticMesh> PlaneMesh(TEXT("/Game/Assets/Meshes/SM_Depot"));
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMesh0"));
    Mesh->SetStaticMesh(PlaneMesh.Get());
    SetRootComponent(Mesh);

    Inventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));
    Inventory->GetInputs().Emplace(50000);
    Inventory->SetBuffer();
}

void ADepot::BeginPlay() {
    Super::BeginPlay();
}


void UDepotUI::Tick() {
    if (Depot) {
        FInventorySlot* inv = &Depot->Inventory->GetInputs()[0];
        InventorySlot->Set(inv);
    }
}
