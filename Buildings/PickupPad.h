// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Building.h"
#include "XD/SelectedUI.h"
#include "XD/Components/InventoryComponent.h"
#include "XD/InventorySlotUI.h"

#include "CoreMinimal.h"
#include "PickupPad.generated.h"

UCLASS()
class XD_API APickupPad : public ABuilding {
    GENERATED_BODY()

public:
    APickupPad();

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UInventoryComponent* Inventory;


    UPROPERTY(EditAnywhere)
    UStaticMeshComponent* Mesh;

protected:
    virtual void BeginPlay() override;

public:

    virtual void OnConstructionComplete() override;
};


UCLASS(Abstract)
class XD_API UPickupPadUI : public USelectedUI {
    GENERATED_BODY()

protected:
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UInventorySlotUI* InventorySlot_1;
    
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UInventorySlotUI* InventorySlot_2;
    
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UInventorySlotUI* InventorySlot_3;
    
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UInventorySlotUI* InventorySlot_4;

public:
    APickupPad* PickupPad;

    virtual void Tick() override;

};
