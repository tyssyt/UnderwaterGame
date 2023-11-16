// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Building.h"
#include "CoreMinimal.h"
#include "Smelter.generated.h"

UCLASS()
class XD_API ASmelter : public ABuilding {
    GENERATED_BODY()

public:
    ASmelter();

    UPROPERTY(EditAnywhere)
    UStaticMeshComponent* Mesh;
};
