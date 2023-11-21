// Fill out your copyright notice in the Description page of Project Settings.

#include "Smelter.h"

#include "XD/Inventory/ConveyorGate.h"

ASmelter::ASmelter() {
    PrimaryActorTick.bCanEverTick = true;

    const static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshFinder(TEXT("/Game/Assets/Meshes/Smelter/Smelter"));
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMesh"));
    Mesh->SetStaticMesh(MeshFinder.Object);
    Mesh->SetRenderCustomDepth(true);
    SetRootComponent(Mesh);

    const auto inConveyorGate = CreateDefaultSubobject<UConveyorGate>(TEXT("InConveyorGate"));
    inConveyorGate->SetInput(true);
    inConveyorGate->SetRelativeLocation(FVector(35., 0., 8.));
    inConveyorGate->SetRenderCustomDepth(true);
    inConveyorGate->SetupAttachment(Mesh);

    const auto outConveyorGate = CreateDefaultSubobject<UConveyorGate>(TEXT("OutConveyorGate"));
    outConveyorGate->SetRelativeLocation(FVector(-35., 20., 8.));
    outConveyorGate->SetRelativeRotation(FRotator(0., 180., 0.));
    outConveyorGate->SetRenderCustomDepth(true);
    outConveyorGate->SetupAttachment(Mesh);
}
