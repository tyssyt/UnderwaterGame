// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ConstructionPlan.h"
#include "XActor.h"
#include "Components/Image.h"
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
    UObject* Source;
    UPROPERTY()
    UTexture2D* Symbol;
    UPROPERTY()
    UMaterialInterface* Material;
    UPROPERTY()
    UMaterialInterface* OverlayMaterial;
    EType Type;
    // TODO maybe a text that shows when hovered
    // TODO maybe even a link into the encyclopedia

public:
    UPROPERTY()
    UImage* Image;

    UCondition* WithSource(UObject* source) { Source = source; return this; }

    UObject* GetSource() const { return Source; }
    UTexture2D* GetSymbol() const { return Symbol; }
    UMaterialInterface* GetMaterial() const { return Material; }
    UMaterialInterface* GetOverlayMaterial() const { return OverlayMaterial; }
    virtual USelectedUI* GetSelectedUI() { return nullptr; } // only for Type NonInteractable
    EType GetType() const { return Type; }

    bool DisablesTick() const;
};

UCLASS()
class XD_API ABuilding : public AXActor {
    GENERATED_BODY()

protected:
    UPROPERTY()
    TArray<UCondition*> Conditions;    
    UPROPERTY()
    UConstructionPlan* ConstructionPlan;
    
    virtual ABuilding* Init(UConstructionPlan* constructionPlan);

public:
    static ABuilding* Spawn(UWorld* world, UConstructionPlan* constructionPlan);
    ABuilding() {}

    UConstructionPlan* GetConstructionPlan() { return ConstructionPlan; }

    virtual void OnConstructionComplete(UBuilderModeExtensions* extensions);
    virtual void OnDismantleStart();
    virtual void OnDismantleFinish();

    void AddCondition(UCondition* condition);
    void RemoveConditions(const UObject* source);
    void RemoveCondition(UCondition* condition);
    UCondition* IsNonInteractable() const;

    virtual UBuilderModeExtensions* CreateBuilderModeExtension();

    virtual void InitSelectedUI(TArray<UBuildingSelectedUIComponent*>& components);
};
