// Fill out your copyright notice in the Description page of Project Settings.

#include "Depot.h"

ADepot::ADepot() {
    const static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshFinder(TEXT("/Game/Assets/Meshes/Depot"));
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMesh0"));
    Mesh->SetStaticMesh(MeshFinder.Object);
    Mesh->SetRenderCustomDepth(true);
    SetRootComponent(Mesh);

    Inventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));
    Inventory->GetInputs().Emplace(40000);
    Inventory->SetBuffer();
    Inventory->AddToUI = true;
}
