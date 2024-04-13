// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "XD/ComponentX.h"
#include "XD/Buildings/BuildingSelectedUI.h"
#include "XD/Resources/Resource.h"
#include "XD/Resources/ResourceBalanceUI.h"
#include "ElectricComponent.generated.h"

class ASubstation;

enum class PowerState { Initial, Disconnected, Deactivated, Unpowered, Powered };

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class XD_API UElectricComponent : public UComponentX {
    GENERATED_BODY()

public:
    enum class Type { IndoorBuilding, OutdoorBuilding, Habitat };

protected:
    PowerState State;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    ASubstation* Substation;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UBillboardComponent* DisabledSymbol;

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int Consumption;

    UElectricComponent();
    UElectricComponent* Init(int consumption);

    Type GetType() const;
    PowerState GetState() const { return State; }
    ASubstation* GetSubstation() const;

    UResource* GetElectricity() const;

    // weird names to not clash with parent functions
    void SetDisconnected();
    void SetConnected(ASubstation* substation);
    void SetDeactivated();
    void SetUnpowered();
    void SetPowered();

    virtual UBuilderModeExtension* CreateBuilderModeExtension() override;
    virtual void OnConstructionComplete(UBuilderModeExtension* extension) override;

    virtual void AddToSelectedUI(TArray<UBuildingSelectedUIComponent*>& components) override;

    friend class UElectricComponentUI;
};

UCLASS()
class XD_API UElectricComponentUI : public UBuildingSelectedUIComponent {
    GENERATED_BODY()

protected:
    UPROPERTY()
    UElectricComponent* ElectricComponent;
    UPROPERTY()
    UResourceBalanceUI* UI;

public:
    UElectricComponentUI* Init(UElectricComponent* electricComponent);
    virtual void CreateUI(UBuildingSelectedUI* selectedUI) override;
    virtual void Tick(UBuildingSelectedUI* selectedUI) override;
};
