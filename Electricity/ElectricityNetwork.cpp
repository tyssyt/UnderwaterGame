// Fill out your copyright notice in the Description page of Project Settings.

#include "ElectricityNetwork.h"

#include "ElectricityManager.h"
#include "PowerOverlay.h"
#include "The.h"
#include "XD/CameraPawn.h"
#include "XD/Buildings/Habitat.h"
#include "XD/Buildings/Substation.h"

ElectricityNetwork::ElectricityNetwork(ASubstation* substation) {
    substations.Add(substation);
    The::ElectricityManager(substation)->ElectricityNetworks.Add(this);
}

ElectricityNetwork::~ElectricityNetwork() {
    The::ElectricityManager(substations[0])->ElectricityNetworks.Remove(this);
}

void ElectricityNetwork::RecomputeStats() {
    TotalConstantProduction = 0;
    TotalConstantConsumption = 0;

    TArray<UElectricComponent*> unpowered = CollectStats();
    UnpowerBuildings(unpowered);

    The::CameraPawn(substations[0])->PowerOverlay->OnNetworkUpdate();
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
    
    TArray<ASubstation*> network = TArray(substations);

    while (network.Num() > 0) {
        TArray<ASubstation*> shard = FindShard(network);

        if (network.Num() > 0) {
            const auto shardNetwork = new ElectricityNetwork(shard[0]);
            for (const auto substation : shard)
                substation->Network = shardNetwork;
            shardNetwork->RecomputeStats();
        } else { // the last shard can keep this network
            this->substations = shard;
            this->RecomputeStats();
        }
    }
}

void ElectricityNetwork::MergeNetwork(const ElectricityNetwork* otherNetwork) {
    MergeNetworkNoRecompute(otherNetwork);
    RecomputeStats();
}

void ElectricityNetwork::MergeNetworkNoRecompute(const ElectricityNetwork* otherNetwork) {
    if (this == otherNetwork)
        return;

    substations.Append(otherNetwork->substations);
    for (const auto otherSubstation : otherNetwork->substations)
        otherSubstation->Network = this;
    delete otherNetwork;
    UE_LOG(LogTemp, Warning, TEXT("Networks Connected."));
}

bool ElectricityNetwork::CollectStats(UElectricComponent* elec) {
    if (elec->GetState() == PowerState::Deactivated)
        return false;

    const int consumption = elec->Consumption;
    if (consumption > 0)
        TotalConstantConsumption += consumption;
    else
        TotalConstantProduction += -consumption;

    return elec->GetState() == PowerState::Unpowered;
}

TArray<UElectricComponent*> ElectricityNetwork::CollectStats() {
    TArray<UElectricComponent*> unpowered;
    for (const auto substation : substations) {
        for (const auto elec : substation->ConnectedBuildings)
            if (CollectStats(elec))
                unpowered.Add(elec);

        for (const auto habitat : substation->ConnectedHabitats) {
            if (CollectStats(habitat))
                unpowered.Add(habitat);

            for (const auto building : habitat->GetOwner<AHabitat>()->Buildings)
                if (const auto elec = building->GetComponentByClass<UElectricComponent>())
                    if (CollectStats(elec))
                        unpowered.Add(elec);    
        }
    }
    return MoveTemp(unpowered);
}

int UnpowerBuilding(UElectricComponent* elec) {
    if (elec->Consumption > 0 && elec->GetState() == PowerState::Powered) {
        elec->SetUnpowered();
        return elec->Consumption;
    }
    return 0;
}

void ElectricityNetwork::UnpowerBuildings(TArray<UElectricComponent*>& unpowered) {
    if (TotalConstantConsumption > TotalConstantProduction) {
        int reducedConsumption = TotalConstantConsumption;

        // prefer to keep those unpowered that already were
        while (reducedConsumption > TotalConstantProduction && !unpowered.IsEmpty()) {
            reducedConsumption -= unpowered.Last()->Consumption;
            unpowered.RemoveAt(unpowered.Num()-1);
        }

        // unpower buildings that were powered
        if (reducedConsumption > TotalConstantProduction) {
            // TODO I hate how this code turned out, I have to find some way to do this that does not suck
            for (const auto substation : substations) {
                for (const auto elec : substation->ConnectedBuildings) {
                    reducedConsumption -= UnpowerBuilding(elec);
                    if (reducedConsumption <= TotalConstantProduction)
                        return;
                }
            }
            for (const auto substation : substations) {
                for (const auto habitat : substation->ConnectedHabitats) {
                    for (const auto building : habitat->GetOwner<AHabitat>()->Buildings) {
                        if (const auto elec = building->GetComponentByClass<UElectricComponent>()) {
                            reducedConsumption -= UnpowerBuilding(elec);
                            if (reducedConsumption <= TotalConstantProduction)
                                return;
                        }
                    }
                }
            }            
            for (const auto substation : substations) {
                for (const auto habitat : substation->ConnectedHabitats) {
                    reducedConsumption -= UnpowerBuilding(habitat);
                    if (reducedConsumption <= TotalConstantProduction)
                        return;
                }
            }
        }
    }

    // turn on all unpowered remaining in the list
    for (const auto up : unpowered) {
        up->SetPowered();
    }
}
