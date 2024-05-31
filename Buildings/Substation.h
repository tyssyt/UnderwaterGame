// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Building.h"
#include "XD/Electricity/ElectricComponent.h"

#include "Components/StaticMeshComponent.h"
#include "CoreMinimal.h"
#include "Components/ProgressBar.h"
#include "Containers/Array.h"
#include "XD/Construction/BuilderModeExtension.h"
#include "XD/Electricity/ElectricityNetwork.h"
#include "XD/Electricity/PowerUI.h"
#include "XD/Electricity/WireComponent.h"
#include "Substation.generated.h"

UCLASS()
class XD_API ASubstation : public ABuilding {
    GENERATED_BODY()

protected:
    void Add(UElectricComponent* building);
    void Remove(UElectricComponent* building);

public:
    ASubstation();

    UPROPERTY(EditAnywhere)
    UStaticMeshComponent* Mesh;

    ElectricityNetwork* Network;

    UPROPERTY(EditAnywhere)
    TArray<UElectricComponent*> ConnectedBuildings;

    UPROPERTY(EditAnywhere)
    TArray<UElectricComponent*> ConnectedHabitats;

    void Connect(UElectricComponent* building, bool recomputeStats = true);
    void Disconnect(UElectricComponent* building);

    void DisconnectFromNetwork();
    virtual void OnConstructionComplete(UBuilderModeExtensions* extensions) override;
    virtual void OnDismantleStart() override;
    virtual UBuilderModeExtensions* CreateBuilderModeExtension() override;

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

UCLASS()
class XD_API USubstationBuilderModeExtension : public UBuilderModeExtension {
    GENERATED_BODY()
    
    TPair<FVector, FRotator> Last;

    UPROPERTY()
    ASubstation* Preview;
    UPROPERTY()
    UConstructionUI* ConstructionUI;
    UPROPERTY()
    TArray<UWireComponent*> Wires;

public:
    UPROPERTY()
    bool AutoConnectWires;

    virtual void Init(ABuilding* preview, UConstructionUI* constructionUI) override;
    virtual void Update() override;
    virtual void End(bool cancelled) override;
};
