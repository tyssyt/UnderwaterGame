// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ConstructionPlan.h"
#include "XActor.h"
#include "XD/Construction/ConstructionOptions.h"
#include "Building.generated.h"

class UBuildingSelectedUI;
class UBuildingSelectedUIComponent;
class UBuilderModeExtension;

UENUM(BlueprintType)
enum class EConstructionState { BuilderMode, ConstructionSite, Done };

UCLASS()
class XD_API ABuilding : public AXActor {
    GENERATED_BODY()

public:
    ABuilding();

    UPROPERTY(EditAnywhere)
    EConstructionState constructionState;

    virtual ABuilding* Init(UConstructionPlan* constructionPlan);
    virtual void OnConstructionComplete(UConstructionOptions* options);

    virtual TSubclassOf<UBuilderModeExtension> GetBuilderModeExtension() const {
        return nullptr;
    }

    virtual void InitSelectedUI(TArray<UBuildingSelectedUIComponent*>& components);
};
