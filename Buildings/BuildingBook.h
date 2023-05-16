// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ConstructionPlan.h"
#include "UObject/Object.h"
#include "BuildingBook.generated.h"

UCLASS()
class XD_API UBuildingBook : public UObject {
    GENERATED_BODY()
    
public:
    UBuildingBook* Init(const UResourceBook* resourceBook);

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UConstructionPlan* ConductiveOreMine;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UConstructionPlan* Depot;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UConstructionPlan* Habitat;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UConstructionPlan* HardOreMine;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UConstructionPlan* OilFishHarvester;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UConstructionPlan* PickupPad;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UConstructionPlan* Smelter;    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UConstructionPlan* Solar;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UConstructionPlan* Substation;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UConstructionPlan* WorkerHouse;
};
