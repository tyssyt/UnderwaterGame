// Fill out your copyright notice in the Description page of Project Settings.

#include "ElectricityManager.h"

#include "XD/Buildings/Substation.h"

ASubstation* UElectricityManager::FindNearestSubstation(FVector from) {    
    ASubstation* nearest = nullptr;
    double nearestDist = INFINITY;

    for (const ElectricityNetwork* network : ElectricityNetworks) {
        for (ASubstation* substation : network->substations) {
            const double dist = FVector::Distance(from, substation->GetActorLocation());
            if (dist < nearestDist) {
                nearest = substation;
                nearestDist = dist;
            }
        }
    }

    if (nearestDist > ElectricityNetwork::MAX_WIRE_DISTANCE)
        return nullptr;
    return nearest;
}
