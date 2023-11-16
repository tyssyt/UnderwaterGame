// Fill out your copyright notice in the Description page of Project Settings.

#include "Excavator.h"

AExcavator::AExcavator() {
    PrimaryActorTick.bCanEverTick = true;

    const static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshFinder(TEXT("/Game/Assets/Meshes/Excavator/Excavator_Base"));
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMesh0"));
    Mesh->SetStaticMesh(MeshFinder.Object);
    Mesh->SetRenderCustomDepth(true);
    SetRootComponent(Mesh);
    
    const static ConstructorHelpers::FObjectFinder<UStaticMesh> WheelMeshFinder(TEXT("/Game/Assets/Meshes/Excavator/Excavator_Wheel"));
    WheelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMesh1"));
    WheelMesh->SetStaticMesh(WheelMeshFinder.Object);
    WheelMesh->SetRenderCustomDepth(true);
    WheelMesh->SetupAttachment(Mesh);

    // TODO fix the model such that the wheel no longer clips the ground
    //WheelMesh->AddLocalOffset(FVector(102.62, 0., 13.099));
    WheelMesh->SetRelativeScale3D(FVector(0.9, 1, 0.9));
    WheelMesh->AddLocalOffset(FVector(102.62, 0., 15.));
}

void AExcavator::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);

    WheelMesh->AddLocalRotation(FRotator(1., 0., 0.));
}
