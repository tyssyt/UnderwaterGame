// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "InventorySlot.h"

#include "CoreMinimal.h"
#include "Containers/Array.h"
#include "XD/ComponentX.h"
#include "XD/Buildings/BuildingSelectedUI.h"
#include "InventoryComponent.generated.h"

class UInventoryUI;
class URecipe;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class XD_API UInventoryComponent : public UComponentX {
    GENERATED_BODY()

    friend class AConveyor; // to access the outputs when it's a Buffer

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FInventorySlot> Inputs;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FInventorySlot> Outputs;

    bool IsBuffer = false;
    
public:
    UInventoryComponent();

    void SetBuffer();
    TArray<FInventorySlot>& GetInputs();
    TArray<FInventorySlot>& GetOutputs();

    void SetRecipe(URecipe* recipe);

    bool AddToUI = false;
    virtual void AddToSelectedUI(UBuildingSelectedUI* selectedUI) override;
    virtual void UpdateSelectedUI(UBuildingSelectedUI* selectedUI) override;
};

UCLASS()
class XD_API UInventoryComponentSelectedData : public USelectedUIData {
    GENERATED_BODY()

public:
    UPROPERTY()
    UInventoryUI* UI;

    UInventoryComponentSelectedData* Init(UInventoryUI* ui);
};
