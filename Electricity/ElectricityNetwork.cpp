// Fill out your copyright notice in the Description page of Project Settings.


#include "ElectricityNetwork.h"

ElectricityNetwork::ElectricityNetwork(ASubstation* substation) {
    substations.push_back(substation);
}

ElectricityNetwork::~ElectricityNetwork() {}

void ElectricityNetwork::recomputeStats() {
    TotalConstantProduction = 0;
    TotalConstantConsumption = 0;


    std::vector<UElectricComponent*> unpowered;

    for (ASubstation* substation : substations) {
        for (UElectricComponent* elec : substation->ConnectedBuildings) {
            int consumption = elec->Consumption;
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
                            return; // if we get more logic below, we must instead break the outer for
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

void ElectricityNetwork::mergeNetworkNoRecompute(ElectricityNetwork* otherNetwork) {
    if (this == otherNetwork)
        return;

    substations.insert(substations.end(), otherNetwork->substations.begin(), otherNetwork->substations.end());
    for (ASubstation* otherSubstation : otherNetwork->substations) {
        otherSubstation->Network = this;
    }
    delete otherNetwork;
    UE_LOG(LogTemp, Warning, TEXT("Networks Connected."));
}

void ElectricityNetwork::removeSubstation(ASubstation* substation) {
    // TODO
}
