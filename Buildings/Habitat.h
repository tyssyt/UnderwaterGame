// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Building.h"
#include "IndoorBuilding.h"
#include "XD/SelectedUI.h"
#include "XD/Inventory/InventorySlotUI.h"
#include "XD/Inventory/InventoryComponent.h"
#include "XD/Electricity/ElectricComponent.h"
#include "XD/PopulationManager/HabitatPopulationManager.h"

#include <utility>

#include "CoreMinimal.h"
#include "Habitat.generated.h"

UCLASS()
class XD_API AHabitat : public ABuilding {
    GENERATED_BODY()

public:
    AHabitat();
    virtual ~AHabitat() override;

    UPROPERTY(EditAnywhere)
    UStaticMeshComponent* Mesh;

    UPROPERTY(EditAnywhere)
    UInventoryComponent* Inventory;

    UPROPERTY(EditAnywhere)
    UElectricComponent* Electricity;

    UPROPERTY(EditAnywhere)
    TArray<AIndoorBuilding*> Buildings;

    virtual void Tick(float DeltaTime) override;

    
    AIndoorBuilding* getBuildingAt(int x, int y) const;
    FVector getGridCellCenter(int x, int y) const;
    std::pair<bool, Coordinate> findCoordinates(FVector hit) const;
    
    bool canPlaceBuilding(AIndoorBuilding* building) const;
    void placeBuilding(AIndoorBuilding* building);
    void removeBuilding(AIndoorBuilding* building);


protected:
    static const int GRID_SIZE = 16;
    static const float CELL_WIDTH; // TODO this is global for all grids, move it somewhere more appropriate
    
    void setBuildingAt(AIndoorBuilding* building, int x, int y);

    virtual void BeginPlay() override;

    HabitatPopulationManager* PopulationManager;
    AIndoorBuilding* BuildGrid[GRID_SIZE * GRID_SIZE];

    friend class UHabitatUI;
};


UCLASS(Abstract)
class XD_API UHabitatUI : public USelectedUI {
    GENERATED_BODY()

protected:
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UInventorySlotUI* People;
    
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UTextBlock* Workforce;

public:
    AHabitat* Habitat;

    virtual void Tick() override;
};
