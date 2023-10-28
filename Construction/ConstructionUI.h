// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ConstructionMaterialsUI.h"
#include "Blueprint/UserWidget.h"
#include "Components/CheckBox.h"
#include "Components/TextBlock.h"
#include "XD/Buildings/BuildingUI.h"
#include "XD/Buildings/ConstructionPlan.h"
#include "ConstructionUI.generated.h"

class UConstructionManager;
class UConstructionPlan;

UCLASS(Abstract)
class XD_API UConstructionUI : public UUserWidget {
    GENERATED_BODY()

protected:
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UBuildingUI* Building;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UTextBlock* BuildingName;

public:    
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UCheckBox* TogglePower;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UConstructionMaterialsUI* ConstructionMaterials;

    void Init(UConstructionPlan* constructionPlan, const UConstructionManager* constructionManager) const;
};
