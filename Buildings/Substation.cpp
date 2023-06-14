// Fill out your copyright notice in the Description page of Project Settings.

#include "Substation.h"

#include "XD/Electricity/ElectricityNetwork.h"


ASubstation::ASubstation() {
    static ConstructorHelpers::FObjectFinderOptional<UStaticMesh> PlaneMesh (TEXT("/Game/Assets/Meshes/Substation"));
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMesh0"));
    Mesh->SetStaticMesh(PlaneMesh.Get());
    SetRootComponent(Mesh);

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

void ASubstation::OnConstructionComplete() {
    Super::OnConstructionComplete();

    // TODO extract common submethod with the electrical component part
    const static float MAX_WIRE_DISTANCE = 500.f; // TODO propably should be a constant in Substation or ElectricityNetwork
    const static FName NAME_QUERY_PARAMS = FName(TEXT(""));
    FCollisionQueryParams queryParams(NAME_QUERY_PARAMS, false, this);
    FCollisionObjectQueryParams objectqueryParams = FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllObjects); // TODO make a custom collision channel with substations and maybe electricComponents

    TArray<FOverlapResult> overlaps;
    GetWorld()->OverlapMultiByObjectType(overlaps, GetActorLocation(), FQuat::Identity, objectqueryParams, FCollisionShape::MakeSphere(MAX_WIRE_DISTANCE), queryParams);

    for (const FOverlapResult& overlap : overlaps) {
        ASubstation* nearbySubstation = Cast<ASubstation>(overlap.GetActor());
        if (nearbySubstation && nearbySubstation->constructionState == ConstructionState::Done) {
            // we found a Substation nearby, connect
            Network->mergeNetworkNoRecompute(nearbySubstation->Network);
        }

        UElectricComponent* nearbyElec = overlap.GetActor()->FindComponentByClass<UElectricComponent>();
        if (nearbyElec && nearbyElec->GetState() != PowerState::Initial && !nearbyElec->Substation) {
            ConnectNoRecompute(nearbyElec);
        }

    }

    Network->recomputeStats();
}
