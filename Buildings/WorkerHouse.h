// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "IndoorBuilding.h"
#include "XD/Inventory/InventorySlotUI.h"

#include "Components/StaticMeshComponent.h"
#include "CoreMinimal.h"
#include "BuildingSelectedUI.h"
#include "XD/PopulationManager/NeedsSummaryUI.h"
#include "WorkerHouse.generated.h"

UCLASS()
class XD_API AWorkerHouse : public AIndoorBuilding {
    GENERATED_BODY()

public:
    AWorkerHouse();

    UPROPERTY(EditAnywhere)
    UStaticMeshComponent* Mesh;

    // TODO I need a special kind of electric component for indoors, because it should connect to the habitat
    // UPROPERTY(EditAnywhere)
    // UElectricComponent* Electricity;

    UPROPERTY(EditAnywhere)
    int Residents;
    
    static constexpr int RESIDENT_LIMIT = 20;

    virtual const TArray<Coordinate>* GetGridOffsets() override;

    virtual void InitSelectedUI(TArray<UBuildingSelectedUIComponent*>& components) override;
};

UCLASS()
class XD_API UWorkerHouseUI : public UBuildingSelectedUIComponent {
    GENERATED_BODY()

protected:
    UPROPERTY()
    UNeedsSummaryUI* Needs;
    UPROPERTY()
    UInventorySlotUI* People;
    
    UPROPERTY()
    AWorkerHouse* House;

public:
    UWorkerHouseUI* Init(AWorkerHouse* house);
    virtual void CreateUI(UBuildingSelectedUI* selectedUI) override;
    virtual void Tick(UBuildingSelectedUI* selectedUI) override;
};
