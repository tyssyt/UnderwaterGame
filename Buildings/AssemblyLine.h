// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Building.h"

#include "Components/StaticMeshComponent.h"
#include "CoreMinimal.h"
#include "AssemblyLine.generated.h"

UCLASS()
class XD_API AAssemblyLine : public ABuilding {
    GENERATED_BODY()

public:
    AAssemblyLine();

    UPROPERTY(EditAnywhere)
    UStaticMeshComponent* Mesh;
};
