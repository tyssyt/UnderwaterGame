// Fill out your copyright notice in the Description page of Project Settings.

#include "Building.h"

#include "XD/Components/ElectricComponent.h"
#include "Substation.h"

ABuilding::ABuilding() : constructionState(ConstructionState::BuilderMode) {}

void ABuilding::BeginPlay() {
    Super::BeginPlay();
}

void ABuilding::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);
}

void ABuilding::OnConstructionComplete() {    
    constructionState = ConstructionState::Done;
    
    // if it has an Electric Component, try to connect it to a substation
    UElectricComponent* elec = FindComponentByClass<UElectricComponent>();
    if (elec) {
        // TODO extract common submethod with Substation
        const static float MAX_WIRE_DISTANCE = 500.f; // TODO propably should be a constant in Substation or ElectricityNetwork
        static FName NAME_QUERY_PARAMS = FName(TEXT(""));
        FCollisionQueryParams queryParams(NAME_QUERY_PARAMS, false, this);
        FCollisionObjectQueryParams objectqueryParams = FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllObjects); // TODO make a custom collision channel with substations and maybe electricComponents

        TArray<FOverlapResult> overlaps;
        GetWorld()->OverlapMultiByObjectType(overlaps, GetActorLocation(), FQuat::Identity, objectqueryParams, FCollisionShape::MakeSphere(MAX_WIRE_DISTANCE), queryParams);

        bool connected = false;
        for (const FOverlapResult& overlap : overlaps) {
            ASubstation* nearbySubstation = Cast<ASubstation>(overlap.GetActor());
            if (nearbySubstation && nearbySubstation->constructionState == ConstructionState::Done) {
                // we found a Substation nearby, connect
                nearbySubstation->Connect(elec);
                connected = true;
                break;
            }
        }

        if (!connected) {
            elec->SetState(PowerState::Disconnected);
        }
    }
}

