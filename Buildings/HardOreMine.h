// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Building.h"
#include "CoreMinimal.h"
#include "HardOreMine.generated.h"

UCLASS()
class XD_API AHardOreMine : public ABuilding {
    GENERATED_BODY()

public:
    AHardOreMine();

    UPROPERTY(EditAnywhere)
    UStaticMeshComponent* Mesh;
};
