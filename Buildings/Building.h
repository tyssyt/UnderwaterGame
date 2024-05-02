// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ConstructionPlan.h"
#include "XActor.h"
#include "Building.generated.h"

class USelectedUI;
class UBuilderModeExtensions;
class UBuildingSelectedUI;
class UBuildingSelectedUIComponent;

// TODO I think (some) conditions should show in the selected UI of the building
UCLASS(Abstract)
class UCondition : public UObject {
    GENERATED_BODY()

public:
    enum class EType { NonInteractable, TickDisabled, Visual };

protected:
    UPROPERTY()
    UTexture2D* Symbol;
    UPROPERTY()
    UMaterialInterface* Material;
    UPROPERTY()
    USelectedUI* SelectedUI; // only for Type NonInteractable
    EType Type;
    // TODO maybe a text that shows when hovered
    // TODO maybe even a link into the encyclopedia

public:
    UTexture2D* GetSymbol() const { return Symbol; }
    UMaterialInterface* GetMaterial() const { return Material; }
    USelectedUI* GetSelectedUI() const { return SelectedUI; }
    EType GetType() const { return Type; }

    bool DisablesTick() const;
};

UCLASS()
class XD_API ABuilding : public AXActor {
    GENERATED_BODY()

protected:
    UPROPERTY()
    TArray<UCondition*> Conditions;

public:
    ABuilding() {}

    virtual ABuilding* Init(UConstructionPlan* constructionPlan);
    virtual void OnConstructionComplete(UBuilderModeExtensions* extensions);

    void AddCondition(UCondition* condition);
    void RemoveCondition(UCondition* condition);
    UCondition* IsNonInteractable() const;

    virtual UBuilderModeExtensions* CreateBuilderModeExtension();

    virtual void InitSelectedUI(TArray<UBuildingSelectedUIComponent*>& components);
};
