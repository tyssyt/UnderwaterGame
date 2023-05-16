// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Building.h"
#include "XD/Components/ElectricComponent.h"

#include "Components/StaticMeshComponent.h"
#include "CoreMinimal.h"
#include "Containers/Array.h"
#include "Substation.generated.h"

UCLASS()
class XD_API ASubstation : public ABuilding {
    GENERATED_BODY()

public:
    ASubstation();


    UPROPERTY(EditAnywhere)
    UStaticMeshComponent* Mesh;

    class ElectricityNetwork* Network;

    UPROPERTY(EditAnywhere)
    TArray<UElectricComponent*> ConnectedBuildings;

    void Connect(UElectricComponent* building);
    void ConnectNoRecompute(UElectricComponent* building);
    virtual void OnConstructionComplete() override;


protected:
    virtual void BeginPlay() override;

};
