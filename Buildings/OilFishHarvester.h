// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Building.h"
#include "CoreMinimal.h"
#include "OilFishHarvester.generated.h"

UCLASS()
class XD_API AOilFishHarvester : public ABuilding {
    GENERATED_BODY()

public:
    AOilFishHarvester();

    UPROPERTY(EditAnywhere)
    UStaticMeshComponent* Mesh;
};
