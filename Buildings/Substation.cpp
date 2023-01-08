// Fill out your copyright notice in the Description page of Project Settings.

#include "Substation.h"

#include "XD/ElectricityNetwork.h"


ASubstation::ASubstation() {

    // Structure to hold one-time initialization
    struct FConstructorStatics {
        ConstructorHelpers::FObjectFinderOptional<UStaticMesh> PlaneMesh;
        FConstructorStatics() : PlaneMesh(TEXT("/Game/Assets/Meshes/Substation")) {}
    };
    static FConstructorStatics ConstructorStatics;

    // Create static mesh component
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMesh0"));
    Mesh->SetStaticMesh(ConstructorStatics.PlaneMesh.Get());

    Network = new ElectricityNetwork(this);
}

void ASubstation::BeginPlay() {
    Super::BeginPlay();
}

void ASubstation::Connect(UElectricComponent* building) {
    ConnectNoRecompute(building);
    Network->recomputeStats();
}

void ASubstation::ConnectNoRecompute(UElectricComponent* building) {
    check(!building->Substation);
    building->Substation = this;
    if (building->Consumption > 0) {
        building->SetState(PowerState::Unpowered); // set as unpowered, the next recompute of the network will power the building if enough power is in the network
    } else {
        building->SetState(PowerState::Powered); // producers are always powered
    }
    ConnectedBuildings.Add(building);
}
