// Fill out your copyright notice in the Description page of Project Settings.

#include "BiomePopulationManager.h"

void UBiomePopulationManager::AddBuilding(UWorkforceComponent* building) {
    Buildings.Add(building);
    building->SetUnstaffed();
}

void UBiomePopulationManager::RemoveBuilding(UWorkforceComponent* building) {
    Buildings.Remove(building);
    WorkforceRequired -= building->Workers;
}

void UBiomePopulationManager::Tick(float DeltaTime) {   
    // TODO if this never explodes, we can remove it at some point
    if (LastFrameNumberWeTicked == GFrameCounter) {
        checkNoEntry();
        return;
    }
    LastFrameNumberWeTicked = GFrameCounter;

    Population = 0;
    Workforce = 150; // TODO create a constant (potentially modifiable from config) for start workforce
    for (const auto habitat : Habitats) {
        Population += habitat->GetCurrentPop();
        Workforce += habitat->GetWorkforce();
    }

    int availableWorkers = Workforce;
    WorkforceRequired = 0;
    for (const auto building : Buildings) {
        WorkforceRequired += building->Workers;
        if (building->GetState() == EWorkforceState::Staffed)
            availableWorkers -= building->Workers;
    }

    if (availableWorkers <= 0)
        return;

    // check if we can enable anything that was disabled
    for (const auto building : Buildings) {
        if (building->GetState() != EWorkforceState::Unstaffed)
            continue;

        if (building->Workers <= availableWorkers) {
            building->SetStaffed();
            availableWorkers -= building->Workers;
            if (availableWorkers <= 0)
                break;
        }
    }    
}
