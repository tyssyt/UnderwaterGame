// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ConstructionUI.h"
#include "Components/WidgetComponent.h"
#include "UObject/Object.h"
#include "XD/Buildings/Building.h"
#include "XD/Electricity/WireComponent.h"
#include "XD/Resources/ResourceBalanceUI.h"
#include "BuilderModeExtension.generated.h"

class ASubstation;
class AIndoorBuilding;

UCLASS(Abstract)
class XD_API UBuilderModeExtension : public UObject {
    GENERATED_BODY()

public:
    virtual void Init(ABuilding* preview, UConstructionUI* constructionUI) {}
    virtual void Update() {}
    virtual void End(UConstructionOptions* options) {}
};

UCLASS()
class XD_API UElectricityConstructionOption : public UConstructionOption {
    GENERATED_BODY()

public:
    UPROPERTY()
    bool AutoConnectWires;
    UElectricityConstructionOption* Init(bool autoConnectWires);
};

UCLASS()
class XD_API UElectricityBuilderModeExtension : public UBuilderModeExtension {
    GENERATED_BODY()
    
    TPair<FVector, FRotator> Last;

    UPROPERTY()
    ABuilding* Preview;
    UPROPERTY()
    UConstructionUI* ConstructionUI;
    UPROPERTY()
    UWireComponent* WireComponent;
    UPROPERTY()
    UWidgetComponent* PowerUI;
    UPROPERTY()
    UResourceBalanceUI* PowerResourceUI;
    UPROPERTY()
    ASubstation* ConnectedSubstation;    

public:
    virtual void Init(ABuilding* preview, UConstructionUI* constructionUI) override;
    virtual void Update() override;
    virtual void End(UConstructionOptions* options) override;
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
    virtual void Init(ABuilding* preview, UConstructionUI* constructionUI) override;
    virtual void Update() override;
    virtual void End(UConstructionOptions* options) override;
};

UCLASS()
class XD_API UIndoorElectricityBuilderModeExtension : public UBuilderModeExtension {
    GENERATED_BODY()

    UPROPERTY()
    AIndoorBuilding* Preview;
    UPROPERTY()
    UConstructionUI* ConstructionUI;
    UPROPERTY()
    UResourceBalanceUI* PowerResourceUI;  

public:
    virtual void Init(ABuilding* preview, UConstructionUI* constructionUI) override;
    virtual void Update() override;
    virtual void End(UConstructionOptions* options) override;
};