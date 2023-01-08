// Fill out your copyright notice in the Description page of Project Settings.

#include "ConstructionSite.h"

#include "Components/ElectricComponent.h"
#include "Buildings/Substation.h"

ConstructionSite::ConstructionSite(AActor* building, UMaterial* ghostMaterial) : Building(building) {
    // set material to ghost 
    UStaticMeshComponent* mesh = Building->FindComponentByClass<UStaticMeshComponent>();

    OldMaterial = mesh->GetMaterial(0)->GetMaterial();
    mesh->SetMaterial(0, ghostMaterial);

    Building->SetActorTickEnabled(false);
    ABuilding* bbuilding = Cast<ABuilding>(building); // TODO eventually we want the input of this function to be some kind of common class...
    if (bbuilding) {
        bbuilding->constructionState = ConstructionState::ConstructionSite;
    }
}

ConstructionSite::~ConstructionSite() {}

void ConstructionSite::BeginConstruction() {
    UStaticMeshComponent* mesh = Building->FindComponentByClass<UStaticMeshComponent>();
    mesh->SetMaterial(0, OldMaterial);
    Building->SetActorTickEnabled(true);
    ABuilding* bbuilding = Cast<ABuilding>(Building); // TODO eventually we want the input of this function to be some kind of common class...
    if (bbuilding) {
        bbuilding->constructionState = ConstructionState::Done;
    }

    const static float MAX_WIRE_DISTANCE = 500.f; // TODO propably should be a constant in Substation or ElectricityNetwork
    static FName NAME_QUERY_PARAMS = FName(TEXT(""));

    // if it has an Electric Component, try to connect it to a substation
    UElectricComponent* elec = Building->FindComponentByClass<UElectricComponent>();
    if (elec) {

        // TODO extract common submethod
        FCollisionQueryParams queryParams(NAME_QUERY_PARAMS, false, Building);
        FCollisionObjectQueryParams objectqueryParams = FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllObjects); // TODO make a custom collision channel with substations and maybe electricComponents

        TArray<FOverlapResult> overlaps;
        Building->GetWorld()->OverlapMultiByObjectType(overlaps, Building->GetActorLocation(), FQuat::Identity, objectqueryParams, FCollisionShape::MakeSphere(MAX_WIRE_DISTANCE), queryParams);

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

    // TODO move this into substation, propably make a "construction finished" function on Building I can call
    // if it is a substation, connect to nearby Buildings & Substations
    ASubstation* substation = Cast<ASubstation>(Building);
    if (substation) {

        // TODO extract common submethod
        FCollisionQueryParams queryParams(NAME_QUERY_PARAMS, false, Building);
        FCollisionObjectQueryParams objectqueryParams = FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllObjects); // TODO make a custom collision channel with substations and maybe electricComponents

        TArray<FOverlapResult> overlaps;
        Building->GetWorld()->OverlapMultiByObjectType(overlaps, Building->GetActorLocation(), FQuat::Identity, objectqueryParams, FCollisionShape::MakeSphere(MAX_WIRE_DISTANCE), queryParams);

        for (const FOverlapResult& overlap : overlaps) {
            ASubstation* nearbySubstation = Cast<ASubstation>(overlap.GetActor());
            if (nearbySubstation && nearbySubstation->constructionState == ConstructionState::Done) {
                // we found a Substation nearby, connect
                substation->Network->mergeNetworkNoRecompute(nearbySubstation->Network);
            }

            UElectricComponent* nearbyElec = overlap.GetActor()->FindComponentByClass<UElectricComponent>();
            if (nearbyElec && nearbyElec->GetState() != PowerState::Initial && !nearbyElec->Substation) {
                substation->ConnectNoRecompute(nearbyElec);
            }

        }

        substation->Network->recomputeStats();
    }

}
