// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Building.h"
#include "XD/SelectedUI.h"

#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "CoreMinimal.h"
#include "XD/Inventory/InventoryComponent.h"
#include "Excavator.generated.h"

UCLASS()
class XD_API AExcavator : public ABuilding {
    GENERATED_BODY()

public:
    AExcavator();

    UPROPERTY(EditAnywhere)
    int ProductionPerTick;

    UPROPERTY(EditAnywhere)
    UInventoryComponent* Inventory;

    UPROPERTY(EditAnywhere)
    UStaticMeshComponent* Mesh;

    UPROPERTY(EditAnywhere)
    UStaticMeshComponent* WheelMesh;


protected:
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

};


UCLASS(Abstract)
class XD_API UExcavatorUI : public USelectedUI {
    GENERATED_BODY()

protected:
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UTextBlock* ProductionPerTick;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UImage* ResourceImage;

public:
    AExcavator* Excavator;

    virtual void Tick() override;

};