// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Building.h"
#include "XD/Inventory/InventoryComponent.h"
#include "CoreMinimal.h"
#include "Depot.generated.h"

UCLASS()
class XD_API ADepot : public ABuilding {
    GENERATED_BODY()

protected:
    UPROPERTY(EditAnywhere)
    UStaticMeshComponent* Mesh;

public:
    UPROPERTY(EditAnywhere)
    UInventoryComponent* Inventory;

    ADepot();
};
