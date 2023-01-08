// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Building.h"
#include "XD/SelectedUI.h"
#include "XD/Components/InventoryComponent.h"
#include "XD/InventorySlotUI.h"

#include "Components/StaticMeshComponent.h"
#include "CoreMinimal.h"
#include "Depot.generated.h"

UCLASS()
class XD_API ADepot : public ABuilding {
    GENERATED_BODY()

public:
    ADepot();

    UPROPERTY(EditAnywhere)
    UInventoryComponent* Inventory;


    UPROPERTY(EditAnywhere)
    UStaticMeshComponent* Mesh;

protected:
    virtual void BeginPlay() override;

public:
};


UCLASS(Abstract)
class XD_API UDepotUI : public USelectedUI {
    GENERATED_BODY()

protected:
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UInventorySlotUI* InventorySlot;

public:
    ADepot* Depot;

    virtual void Tick() override;

};
