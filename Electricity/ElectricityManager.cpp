// Fill out your copyright notice in the Description page of Project Settings.

#include "ElectricityManager.h"

#include "XD/Buildings/Substation.h"

ASubstation* UElectricityManager::FindNearestSubstation(FVector from) {    
    ASubstation* nearest = nullptr;
    double nearestDist = INFINITY;

    for (const auto network : ElectricityNetworks) {
        if (const auto substation = network->FindNearestSubstation(from)) {
            const double dist = FVector::Distance(from, substation->GetActorLocation());
            if (dist < nearestDist) {
                nearest = substation;
                nearestDist = dist;
            }
        }
    }

    return nearest;
}
