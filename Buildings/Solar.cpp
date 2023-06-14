// Fill out your copyright notice in the Description page of Project Settings.

#include "Solar.h"


ASolar::ASolar() {
    static ConstructorHelpers::FObjectFinderOptional<UStaticMesh> PlaneMesh (TEXT("/Game/Assets/Meshes/Solar"));
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMesh0"));
    Mesh->SetStaticMesh(PlaneMesh.Get());
    SetRootComponent(Mesh);

    Electricity = CreateDefaultSubobject<UElectricComponent>(TEXT("Electricity"));
    Electricity->Consumption = -200;
}

void ASolar::BeginPlay() {
    Super::BeginPlay();
}
