// Fill out your copyright notice in the Description page of Project Settings.

#include "HardOreMine.h"

#include "XD/Inventory/ConveyorGate.h"

AHardOreMine::AHardOreMine() {
    PrimaryActorTick.bCanEverTick = true;

    const static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshFinder(TEXT("/Game/Cylinder"));
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMesh0"));
    Mesh->SetStaticMesh(MeshFinder.Object);
    Mesh->SetRenderCustomDepth(true);
    Mesh->SetRelativeLocation(FVector(0., 0., 50.));
    Mesh->SetRelativeScale3D(FVector(1., 1., .25));
    SetRootComponent(Mesh);

    const auto conveyorGate = CreateDefaultSubobject<UConveyorGate>(TEXT("ConveyorGate"));
    conveyorGate->SetRelativeLocation(FVector(52., 0., 0.));
    conveyorGate->SetRelativeScale3D(FVector(1., 1., 4.));
    conveyorGate->SetRenderCustomDepth(true);
    conveyorGate->SetupAttachment(Mesh);
}
