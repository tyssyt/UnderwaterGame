// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Building.h"
#include "IndoorBuilding.h"
#include "XD/Inventory/InventorySlotUI.h"
#include "XD/Inventory/InventoryComponent.h"
#include "XD/PopulationManager/HabitatPopulationManager.h"

#include <utility>

#include "CoreMinimal.h"
#include "XD/PopulationManager/NeedsSummaryUI.h"
#include "Habitat.generated.h"

UCLASS()
class XD_API AHabitat : public ABuilding {
    GENERATED_BODY()

public:
    AHabitat();

    UPROPERTY(EditAnywhere)
    UStaticMeshComponent* Mesh;

    UPROPERTY(EditAnywhere)
    UInventoryComponent* Inventory;

    UPROPERTY(EditAnywhere)
    TArray<AIndoorBuilding*> Buildings;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UHabitatPopulationManager* PopulationManager;
    
    virtual void OnConstructionComplete(UConstructionOptions* options) override;

    virtual void Tick(float DeltaTime) override;

    
    AIndoorBuilding* GetBuildingAt(int x, int y) const;
    FVector GetGridCellCenter(int x, int y) const;
    TPair<bool, Coordinate> FindCoordinates(FVector hit) const;
    
    bool CanPlaceBuilding(AIndoorBuilding* building) const;
    void PlaceBuilding(AIndoorBuilding* building);
    void RemoveBuilding(AIndoorBuilding* building);

    bool HasBuilding(const UConstructionPlan* building) const;

    virtual void InitSelectedUI(TArray<UBuildingSelectedUIComponent*>& components) override;


protected:
    static const int GRID_SIZE = 16;
    static const float CELL_WIDTH; // TODO this is global for all grids, move it somewhere more appropriate
    
    void SetBuildingAt(AIndoorBuilding* building, int x, int y);

    virtual void BeginPlay() override;
    AIndoorBuilding* BuildGrid[GRID_SIZE * GRID_SIZE];
};


UCLASS()
class XD_API UHabitatUI : public UBuildingSelectedUIComponent {
    GENERATED_BODY()

protected:
    UPROPERTY()
    UNeedsSummaryUI* Needs;
    UPROPERTY()
    UInventorySlotUI* People;
    
    UPROPERTY()
    AHabitat* Habitat;

public:
    UHabitatUI* Init(AHabitat* habitat);
    virtual void CreateUI(UBuildingSelectedUI* selectedUI) override;
    virtual void Tick(UBuildingSelectedUI* selectedUI) override;    
};
