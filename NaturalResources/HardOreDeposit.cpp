// Fill out your copyright notice in the Description page of Project Settings.

#include "HardOreDeposit.h"

AHardOreDeposit::AHardOreDeposit() {
    PrimaryActorTick.bCanEverTick = false;
    
    const static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshFinder(TEXT("/Game/Cylinder"));
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMesh0"));
    Mesh->SetStaticMesh(MeshFinder.Object);
    Mesh->SetRelativeLocation(FVector(0.f, 0.f, 50.f));
    Mesh->SetRelativeScale3D(FVector(1.f, 1.f, .25f));
    SetRootComponent(Mesh);
}
