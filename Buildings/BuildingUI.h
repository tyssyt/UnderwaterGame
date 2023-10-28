// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ConstructionPlan.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "BuildingUI.generated.h"

UCLASS(Abstract)
class XD_API UBuildingUI : public UUserWidget {
    GENERATED_BODY()

protected:
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UImage* Image;

    UPROPERTY(BlueprintReadOnly)
    UConstructionPlan* Building;

public:
    UBuildingUI* Init(UConstructionPlan* building);
    UConstructionPlan* GetBuilding() const;
};
