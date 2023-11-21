// Fill out your copyright notice in the Description page of Project Settings.

#include "AssemblyLine.h"

#include "XD/Inventory/ConveyorGate.h"

AAssemblyLine::AAssemblyLine() {
    PrimaryActorTick.bCanEverTick = true;

    const static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshFinder(TEXT("/Game/Assets/Meshes/AssemblyLine"));
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMesh"));
    Mesh->SetStaticMesh(MeshFinder.Object);
    Mesh->SetRenderCustomDepth(true);
    SetRootComponent(Mesh);

    const auto inConveyorGate1 = CreateDefaultSubobject<UConveyorGate>(TEXT("InConveyorGate1"));
    inConveyorGate1->SetInput(true);
    inConveyorGate1->SetRelativeLocation(FVector(87., 10., 8.));
    inConveyorGate1->SetRenderCustomDepth(true);
    inConveyorGate1->SetupAttachment(Mesh);

    const auto inConveyorGate2 = CreateDefaultSubobject<UConveyorGate>(TEXT("InConveyorGate2"));
    inConveyorGate2->SetInput(true);
    inConveyorGate2->SetRelativeLocation(FVector(87., -30., 8.));
    inConveyorGate2->SetRenderCustomDepth(true);
    inConveyorGate2->SetupAttachment(Mesh);

    const auto outConveyorGate = CreateDefaultSubobject<UConveyorGate>(TEXT("OutConveyorGate"));
    outConveyorGate->SetRelativeLocation(FVector(-144., 10., 8.));
    outConveyorGate->SetRelativeRotation(FRotator(0., 180., 0.));
    outConveyorGate->SetRenderCustomDepth(true);
    outConveyorGate->SetupAttachment(Mesh);
}
