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

    bool bIsBuffer = false;
    
public:
    UInventoryComponent();

    void SetBuffer();
    bool IsBuffer() const { return bIsBuffer; }
    TArray<FInventorySlot>& GetInputs() { return Inputs; }
    TArray<FInventorySlot>& GetOutputs() { return bIsBuffer ? Inputs : Outputs; }

    FInventorySlot* GetInput(const UResource* resource);
    FInventorySlot* GetOutput(const UResource* resource);

    TArray<FInventorySlot*> GetUnconnected(bool isInput);
    void SetConveyor(bool isInput, FInventorySlot* slot, AConveyor* conveyor);

    void SetRecipe(URecipe* recipe);

    virtual void AddToSelectedUI(TArray<UBuildingSelectedUIComponent*>& components) override;
};

UCLASS()
class XD_API UInventoryComponentUI : public UBuildingSelectedUIComponent {
    GENERATED_BODY()

protected:
    UPROPERTY()
    UInventoryUI* UI;
    UPROPERTY()
    UInventoryComponent* InventoryComponent;

public:
    UInventoryComponentUI* Init(UInventoryComponent* inventoryComponent);
    virtual void CreateUI(UBuildingSelectedUI* selectedUI) override;
    virtual void Tick(UBuildingSelectedUI* selectedUI) override;
};
