// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Building.h"
#include "XD/SelectedUI.h"

#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "CoreMinimal.h"
#include "ConductiveOreMine.generated.h"

UCLASS()
class XD_API AConductiveOreMine : public ABuilding {
    GENERATED_BODY()

public:
    AConductiveOreMine();

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
class XD_API UConductiveOreMineUI : public USelectedUI {
    GENERATED_BODY()

protected:
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UTextBlock* ProductionPerTick;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UImage* ResourceImage;

public:
    AConductiveOreMine* Mine;

    virtual void Tick() override;

};
