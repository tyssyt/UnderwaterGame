// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Building.h"
#include "CoreMinimal.h"
#include "Excavator.generated.h"

UCLASS()
class XD_API AExcavator : public ABuilding {
    GENERATED_BODY()

protected:
    UPROPERTY(EditAnywhere)
    UStaticMeshComponent* Mesh;
    UPROPERTY(EditAnywhere)
    UStaticMeshComponent* WheelMesh;
    
public:
    AExcavator();
    virtual void Tick(float DeltaTime) override;
};
