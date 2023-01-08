// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "IndoorBuilding.h"
#include "XD/SelectedUI.h"

#include "Components/StaticMeshComponent.h"
#include "CoreMinimal.h"
#include "WorkerHouse.generated.h"

UCLASS()
class XD_API AWorkerHouse : public AIndoorBuilding {
    GENERATED_BODY()

public:
    AWorkerHouse();

    UPROPERTY(EditAnywhere)
    UStaticMeshComponent* Mesh;

    UPROPERTY(EditAnywhere)
    int Residents;
    
    static constexpr int RESIDENT_LIMIT = 20;

    virtual const std::vector<Coordinate>* getGridOffsets() override;

protected:
    virtual void BeginPlay() override;


public:
};


UCLASS(Abstract)
class XD_API UWorkerHouseUI : public USelectedUI {
    GENERATED_BODY()

protected:
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UInventorySlotUI* People;
    
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UTextBlock* Workforce;

public:
    AWorkerHouse* House;

    virtual void Tick() override;
};
