// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "UObject/Object.h"
#include "XD/Construction/BuilderModeExtension.h"
#include "XD/Construction/ConstructionUI.h"
#include "XD/Electricity/WireComponent.h"
#include "XD/Resources/ResourceBalanceUI.h"
#include "ElectricityBuilderModeExtension.generated.h"

class ASubstation;
class AIndoorBuilding;

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
    UPROPERTY()
    bool AutoConnectWires;

    virtual void Init(ABuilding* preview, UConstructionUI* constructionUI) override;
    virtual void Update() override;
    virtual void End(bool cancelled) override;
};
