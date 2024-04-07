// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Building.h"
#include "XD/Electricity/ElectricComponent.h"

#include "Components/StaticMeshComponent.h"
#include "CoreMinimal.h"
#include "Components/ProgressBar.h"
#include "Containers/Array.h"
#include "XD/SelectedUI.h"
#include "XD/Electricity/ElectricityNetwork.h"
#include "XD/Electricity/PowerUI.h"
#include "Substation.generated.h"

UCLASS()
class XD_API ASubstation : public ABuilding {
    GENERATED_BODY()

public:
    ASubstation();

    UPROPERTY(EditAnywhere)
    UStaticMeshComponent* Mesh;

    ElectricityNetwork* Network;

    UPROPERTY(EditAnywhere)
    TArray<UElectricComponent*> ConnectedBuildings;

    void Connect(UElectricComponent* building);
    void Disconnect(UElectricComponent* building);
    void DisconnectFromNetwork();
    void ConnectNoRecompute(UElectricComponent* building);
    void ReconnectNoRecompute(UElectricComponent* building);
    virtual void OnConstructionComplete(UConstructionOptions* options) override;
    virtual TSubclassOf<UBuilderModeExtension> GetBuilderModeExtension() const override;

    TPair<TArray<ASubstation*>, TArray<UElectricComponent*>> FindNearby() const;

    virtual void InitSelectedUI(TArray<UBuildingSelectedUIComponent*>& components) override;
};


UCLASS(Abstract)
class XD_API USubstationUI : public UUserWidget {
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UPowerUI* PowerUI;
    
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UProgressBar* FillLevel;

};

UCLASS()
class XD_API USubstationUIComponent : public UBuildingSelectedUIComponent {
    GENERATED_BODY()

public:
    UPROPERTY()
    USubstationUI* UI;
    UPROPERTY()
    ASubstation* Substation;

    USubstationUIComponent* Init(ASubstation* substation);
    virtual void CreateUI(UBuildingSelectedUI* selectedUI) override;
    virtual void Tick(UBuildingSelectedUI* selectedUI) override;
};
