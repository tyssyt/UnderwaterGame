// Fill out your copyright notice in the Description page of Project Settings.

#include "Solar.h"


ASolar::ASolar() {

    // Structure to hold one-time initialization
    struct FConstructorStatics {
        ConstructorHelpers::FObjectFinderOptional<UStaticMesh> PlaneMesh;
        FConstructorStatics() : PlaneMesh(TEXT("/Game/Assets/Meshes/Solar")) {}
    };
    static FConstructorStatics ConstructorStatics;

    // Create static mesh component
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMesh0"));
    Mesh->SetStaticMesh(ConstructorStatics.PlaneMesh.Get());

    Electricity = CreateDefaultSubobject<UElectricComponent>(TEXT("Electricity"));
    Electricity->Consumption = -200;
}

void ASolar::BeginPlay() {
    Super::BeginPlay();
}
