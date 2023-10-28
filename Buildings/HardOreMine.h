// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Building.h"
#include "XD/SelectedUI.h"

#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "CoreMinimal.h"
#include "XD/Inventory/InventoryComponent.h"
#include "HardOreMine.generated.h"

UCLASS()
class XD_API AHardOreMine : public ABuilding {
    GENERATED_BODY()

public:
    AHardOreMine();

    UPROPERTY(EditAnywhere)
    int ProductionPerTick;

    UPROPERTY(EditAnywhere)
    UInventoryComponent* Inventory;

    UPROPERTY(EditAnywhere)
    UStaticMeshComponent* Mesh;


protected:
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

};

UCLASS(Abstract)
class XD_API UHardOreMineUI : public USelectedUI {
    GENERATED_BODY()

protected:
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UTextBlock* ProductionPerTick;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UImage* ResourceImage;

public:
    AHardOreMine* Mine;

    virtual void Tick() override;

};
