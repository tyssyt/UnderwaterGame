// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "XD/ComponentX.h"
#include "XD/Construction/ConstructionOptions.h"
#include "Components/BillboardComponent.h"
#include "XD/Buildings/BuildingSelectedUI.h"
#include "XD/Resources/Resource.h"
#include "XD/Resources/ResourceBalanceUI.h"
#include "ElectricComponent.generated.h"

class ASubstation;

enum class PowerState { Initial, Disconnected, Deactivated, Unpowered, Powered };

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class XD_API UElectricComponent : public UComponentX {
    GENERATED_BODY()

protected:
    PowerState State;

public:
    UPROPERTY(EditAnywhere)
    int Consumption;

    UPROPERTY(EditAnywhere)
    ASubstation* Substation;

    UPROPERTY(EditAnywhere)
    UBillboardComponent* DisabledSymbol;

    UElectricComponent();

    PowerState GetState() const;
    void SetState(const PowerState newState);

    UResource* GetElectricity() const;

    virtual TSubclassOf<UBuilderModeExtension> GetBuilderModeExtension() const override;
    virtual void OnConstructionComplete(UConstructionOptions* options) override;
    virtual void AddToSelectedUI(UBuildingSelectedUI* selectedUI) override;
    virtual void UpdateSelectedUI(UBuildingSelectedUI* selectedUI) override;
};

UCLASS()
class XD_API UElectricComponentSelectedData : public USelectedUIData {
    GENERATED_BODY()

public:
    UPROPERTY()
    UResourceBalanceUI* UI;

    UElectricComponentSelectedData* Init(UResourceBalanceUI* ui);
    
};
