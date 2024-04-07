// Fill out your copyright notice in the Description page of Project Settings.


#include "NeedSatisfier.h"

#include "XD/Buildings/Habitat.h"
#include "XD/Buildings/IndoorBuilding.h"

int UNeedSatisfier::IsSatisfied(const AHabitat* habitat, int population) {
    for (const auto service : Services) {
        if (service->BuildingClass->IsChildOf<AIndoorBuilding>()) {            
            if (!habitat->HasBuilding(service))
                return 0;
        } else {
            // TODO search in the biome
            UE_LOG(LogTemp, Error, TEXT("Service that is not an indoor Building"));
            checkNoEntry();
            return 0;
        }
    }

    int satisfiedPop = population;
    for (const auto& good : Goods)
        satisfiedPop = FMath::Min(satisfiedPop, good.IsSatisfied(habitat, population));
    return satisfiedPop;
}
