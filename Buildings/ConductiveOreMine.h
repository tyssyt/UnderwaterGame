// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Building.h"
#include "CoreMinimal.h"
#include "ConductiveOreMine.generated.h"

UCLASS()
class XD_API AConductiveOreMine : public ABuilding {
    GENERATED_BODY()

public:
    AConductiveOreMine();

    UPROPERTY(EditAnywhere)
    UStaticMeshComponent* Mesh;
};
