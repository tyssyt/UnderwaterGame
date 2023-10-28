// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BuildingUI.h"
#include "Blueprint/UserWidget.h"
#include "Components/WrapBox.h"
#include "XD/NaturalResources/NaturalResourceUI.h"
#include "XD/Resources/ResourceAmountUI.h"
#include "ConstructionPlanUI.generated.h"

UCLASS(Abstract)
class XD_API UConstructionPlanUI : public UUserWidget {
    GENERATED_BODY()

protected:
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UWrapBox* Materials;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UBuildingUI* Building;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UResourceAmountUI> ResourceAmountUIClass;
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UBuildingUI> BuildingUIClass;
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UNaturalResourceUI> NaturalResourceUIClass;

public:
    UConstructionPlanUI* Init(UConstructionPlan* constructionPlan, bool includeNeeds);
};
