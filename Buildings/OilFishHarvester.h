// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Building.h"
#include "XD/SelectedUI.h"
#include "XD/Components/InventoryComponent.h"

#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "CoreMinimal.h"
#include "OilFishHarvester.generated.h"

UCLASS()
class XD_API AOilFishHarvester : public ABuilding {
    GENERATED_BODY()

public:
    AOilFishHarvester();

    UPROPERTY(EditAnywhere)
    int ProductionPerTick;

    UPROPERTY(EditAnywhere)
    class UInventoryComponent* Inventory;

    UPROPERTY(EditAnywhere)
    UStaticMeshComponent* Mesh;


protected:
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

};

UCLASS(Abstract)
class XD_API UOilFishHarvesterUI : public USelectedUI {
    GENERATED_BODY()

protected:
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UTextBlock* ProductionPerTick;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UImage* ResourceImage;

public:
    AOilFishHarvester* Mine;

    virtual void Tick() override;

};
