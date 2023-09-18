// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ElectricityNetwork.h"
#include "UObject/Object.h"
#include "ElectricityManager.generated.h"

UCLASS()
class XD_API UElectricityManager : public UObject {
    GENERATED_BODY()

public:
    TArray<ElectricityNetwork*> ElectricityNetworks;
    
    UPROPERTY(EditAnywhere)
    TArray<UElectricComponent*> Disconnected;

    ASubstation* FindNearestSubstation(FVector from);
};
