// Fill out your copyright notice in the Description page of Project Settings.

#include "Depot.h"

#include "XD/Inventory/ConveyorGate.h"

ADepot::ADepot() {
    const static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshFinder(TEXT("/Game/Assets/Meshes/Depot"));
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMesh0"));
    Mesh->SetStaticMesh(MeshFinder.Object);
    Mesh->SetRenderCustomDepth(true);
    SetRootComponent(Mesh);

    const auto inConveyorGate = CreateDefaultSubobject<UConveyorGate>(TEXT("InConveyorGate"));
    inConveyorGate->SetInput(true);
    inConveyorGate->SetRelativeLocation(FVector(20., 0., 8.));
    inConveyorGate->SetRenderCustomDepth(true);
    inConveyorGate->SetupAttachment(Mesh);

    const auto outConveyorGate = CreateDefaultSubobject<UConveyorGate>(TEXT("OutConveyorGate"));
    outConveyorGate->SetRelativeLocation(FVector(-20., 0., 8.));
    outConveyorGate->SetRelativeRotation(FRotator(0., 180., 0.));
    outConveyorGate->SetRenderCustomDepth(true);
    outConveyorGate->SetupAttachment(Mesh);

    Inventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));
    Inventory->GetInputs().Emplace(1000);
    Inventory->SetBuffer();
    Inventory->AddToUI = true;
}
