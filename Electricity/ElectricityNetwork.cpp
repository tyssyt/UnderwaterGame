// Fill out your copyright notice in the Description page of Project Settings.

#include "ElectricityNetwork.h"

#include "PowerOverlay.h"
#include "XD/CameraPawn.h"
#include "XD/GameInstanceX.h"
#include "XD/PlayerControllerX.h"

ElectricityNetwork::ElectricityNetwork(ASubstation* substation) {
    substations.Add(substation);
    substation->GetGameInstance()->TheElectricityManager->ElectricityNetworks.Add(this);
}

ElectricityNetwork::~ElectricityNetwork() {
    substations[0]->GetGameInstance()->TheElectricityManager->ElectricityNetworks.Remove(this);
}

void ElectricityNetwork::RecomputeStats() {
    TotalConstantProduction = 0;
    TotalConstantConsumption = 0;


    std::vector<UElectricComponent*> unpowered = CollectStats();
    UnpowerBuildings(unpowered);

    const ACameraPawn* cameraPawn = substations[0]->GetWorld()->GetFirstPlayerController<APlayerControllerX>()->GetPawn<ACameraPawn>();
    cameraPawn->PowerOverlay->OnNetworkUpdate();
}

TArray<ASubstation*> FindShard(TArray<ASubstation*>& network) {
    TArray<ASubstation*> shard;
    TArray<ASubstation*> queue;

    {
        const auto first = network.Pop();
        shard.Add(first);
        queue.Push(first);
    }

    while (queue.Num() > 0) {
        auto current = queue.Pop()->GetActorLocation();
        
        for (int i = network.Num()-1; i >= 0; --i) {            
            if (FVector::Distance(current, network[i]->GetActorLocation()) > ElectricityNetwork::MAX_WIRE_DISTANCE)
                continue;
            shard.Add(network[i]);
            queue.Push(network[i]);
            network.RemoveAtSwap(i, 1, false);
        }
    }
    return shard;
}

void ElectricityNetwork::CheckForNetworkSplit() { // TODO there are much faster algos for this, this is n²
    if (substations.Num() < 2) {
        this->RecomputeStats();
        return;
    }
    
    TArray<ASubstation*> network = TArray<ASubstation*>(substations);

    while (network.Num() > 0) {
        TArray<ASubstation*> shard = FindShard(network);

        if (network.Num() > 0) {
            ElectricityNetwork* shardNetwork = new ElectricityNetwork(shard[0]);
            for (const auto substation : shard)
                substation->Network = shardNetwork;
            shardNetwork->RecomputeStats();
        } else { // the last shard can keep this network
            this->substations = shard;
            this->RecomputeStats();
        }
    }
}

void ElectricityNetwork::MergeNetwork(ElectricityNetwork* otherNetwork) {
    MergeNetworkNoRecompute(otherNetwork);
    RecomputeStats();
}

void ElectricityNetwork::MergeNetworkNoRecompute(ElectricityNetwork* otherNetwork) {
    if (this == otherNetwork)
        return;

    substations.Append(otherNetwork->substations);
    for (ASubstation* otherSubstation : otherNetwork->substations) {
        otherSubstation->Network = this;
    }
    delete otherNetwork;
    UE_LOG(LogTemp, Warning, TEXT("Networks Connected."));
}

std::vector<UElectricComponent*> ElectricityNetwork::CollectStats() {
    std::vector<UElectricComponent*> unpowered;
    for (ASubstation* substation : substations) {
        for (UElectricComponent* elec : substation->ConnectedBuildings) {
            if (elec->GetState() == PowerState::Deactivated)
                continue;
            
            const int consumption = elec->Consumption;
            if (consumption > 0) {
                TotalConstantConsumption += consumption;
            } else {
                TotalConstantProduction += -consumption;
            }

            if (elec->GetState() == PowerState::Unpowered) {
                unpowered.push_back(elec);
            }
        }
    }
    return unpowered;
}

void ElectricityNetwork::UnpowerBuildings(std::vector<UElectricComponent*>& unpowered) {
    if (TotalConstantConsumption > TotalConstantProduction) {
        int reducedConsumption = TotalConstantConsumption;

        // prefer to keep those unpowered that already were
        while (reducedConsumption > TotalConstantProduction && !unpowered.empty()) {
            reducedConsumption -= unpowered.back()->Consumption;
            unpowered.pop_back();
        }

        // unpower buildings that were powered
        if (reducedConsumption > TotalConstantProduction) {
            for (ASubstation* substation : substations) {
                for (UElectricComponent* elec : substation->ConnectedBuildings) {
                    if (elec->Consumption > 0 && elec->GetState() == PowerState::Powered) {
                        reducedConsumption -= elec->Consumption;
                        elec->SetState(PowerState::Unpowered);
                        if (reducedConsumption <= TotalConstantProduction) {
                            return;
                        }
                    }
                }
            }
        }
    }

    // turn on all unpowered remaining in the list
    for (UElectricComponent* up : unpowered) {
        up->SetState(PowerState::Powered);
    }
}
