// Fill out your copyright notice in the Description page of Project Settings.

#include "Substation.h"

#include "Components/ProgressBar.h"
#include "XD/Electricity/ElectricityNetwork.h"

ASubstation::ASubstation() {
    const static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshFinder(TEXT("/Game/Assets/Meshes/Substation"));
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMesh0"));
    Mesh->SetStaticMesh(MeshFinder.Object);
    SetRootComponent(Mesh);
}

void ASubstation::BeginPlay() {
    Super::BeginPlay();
}

void ASubstation::Connect(UElectricComponent* building) {
    ConnectNoRecompute(building);
    Network->RecomputeStats();
}

void ASubstation::Disconnect(UElectricComponent* building) {
    check(building->Substation == this);
    building->Substation = nullptr;
    building->SetState(PowerState::Disconnected);
    ConnectedBuildings.Remove(building);
    Network->RecomputeStats();
}

void ASubstation::DisconnectFromNetwork() {
    check(Network->substations.Num() > 1);
    ElectricityNetwork* old = Network;
    Network = new ElectricityNetwork(this);
    old->substations.Remove(this);
    old->CheckForNetworkSplit();
    Network->RecomputeStats();
}

void ASubstation::ConnectNoRecompute(UElectricComponent* building) {
    check(!building->Substation);
    building->Substation = this;
    if (building->Consumption > 0)
        building->SetState(PowerState::Unpowered); // set as unpowered, the next recompute of the network will power the building if enough power is in the network
    else
        building->SetState(PowerState::Powered); // producers are always powered
    ConnectedBuildings.Add(building);
}

void ASubstation::ReconnectNoRecompute(UElectricComponent* building) {
    check(building->Substation);
    check(Network == building->Substation->Network);
    building->Substation->ConnectedBuildings.Remove(building);
    building->Substation = this;
    ConnectedBuildings.Add(building);
}

void ASubstation::OnConstructionComplete(FConstructionFlags flags) {
    Super::OnConstructionComplete(flags);

    if (!flags.autoConnectWires) {
        Network = new ElectricityNetwork(this);
        Network->RecomputeStats();
        return;
    }

    const auto nearby = FindNearby();
    if (!nearby.first.empty()) {
        // join & merge nearby networks
        Network = nearby.first.front()->Network;
        Network->substations.Add(this);
        for (const ASubstation* nearbySubstation : nearby.first) {
            Network->MergeNetworkNoRecompute(nearbySubstation->Network);
        }
    } else
        Network = new ElectricityNetwork(this);
    
    for (UElectricComponent* nearbyElec : nearby.second) {
        if (nearbyElec->Substation) {
            // check if we are on the same network and closer then the substation currently used
            FVector elecLocation = nearbyElec->GetOwner()->GetActorLocation();
            if (nearbyElec->Substation->Network == Network && FVector::Distance(GetActorLocation(), elecLocation) < FVector::Distance(nearbyElec->Substation->GetActorLocation(), elecLocation)) {
                ReconnectNoRecompute(nearbyElec);
            }
        } else // Always connect unconnected components            
            ConnectNoRecompute(nearbyElec);
    }

    Network->RecomputeStats();
}

std::pair<std::vector<ASubstation*>, std::vector<UElectricComponent*>> ASubstation::FindNearby() const {    
    const static FName NAME_QUERY_PARAMS = FName(TEXT(""));
    const FCollisionQueryParams queryParams(NAME_QUERY_PARAMS, false, this);
    const FCollisionObjectQueryParams objectQueryParams = FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllObjects); // TODO make a custom collision channel with substations and maybe electricComponents

    TArray<FOverlapResult> overlaps;
    GetWorld()->OverlapMultiByObjectType(overlaps, GetActorLocation(), FQuat::Identity, objectQueryParams, FCollisionShape::MakeSphere(ElectricityNetwork::MAX_WIRE_DISTANCE), queryParams);

    std::vector<ASubstation*> nearbySubstations;
    std::vector<UElectricComponent*> nearbyElecs;
    for (const FOverlapResult& overlap : overlaps) {
        ASubstation* nearbySubstation = Cast<ASubstation>(overlap.GetActor());
        if (nearbySubstation && nearbySubstation->constructionState == EConstructionState::Done && FVector::Distance(GetActorLocation(), nearbySubstation->GetActorLocation()) < ElectricityNetwork::MAX_WIRE_DISTANCE) {
            nearbySubstations.push_back(nearbySubstation);
        }

        UElectricComponent* nearbyElec = overlap.GetActor()->FindComponentByClass<UElectricComponent>();
        if (nearbyElec && nearbyElec->GetState() != PowerState::Initial && FVector::Distance(GetActorLocation(), nearbyElec->GetOwner()->GetActorLocation()) < ElectricityNetwork::MAX_WIRE_DISTANCE) {
            nearbyElecs.push_back(nearbyElec);
        }
    }

    return std::make_pair(nearbySubstations, nearbyElecs);
}

void USubstationUI::Tick() {
    if (Substation) {
        if (Substation->Network) {
            const int production = Substation->Network->GetTotalConstantProduction();
            const int consumption = Substation->Network->GetTotalConstantConsumption();

            PowerUI->Set(production, consumption);
            FillLevel->SetPercent(static_cast<float>(consumption) / production);
        } else {
            PowerUI->Set(0, 0);
            FillLevel->SetPercent(0.);
        }
    }
}
