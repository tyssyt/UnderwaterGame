// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Building.h"

#include "Components/StaticMeshComponent.h"
#include "CoreMinimal.h"
#include "Solar.generated.h"

UCLASS()
class XD_API ASolar : public ABuilding {
    GENERATED_BODY()

public:
    ASolar();

    UPROPERTY(EditAnywhere)
    UStaticMeshComponent* Mesh;
};
