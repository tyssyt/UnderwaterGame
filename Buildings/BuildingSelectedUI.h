// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BuildingUI.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/WrapBox.h"
#include "XD/SelectedUI.h"
#include "BuildingSelectedUI.generated.h"

class UBuildingSelectedUIComponent;

UCLASS(Abstract)
class XD_API UBuildingSelectedUI : public USelectedUI {
    GENERATED_BODY()

protected:
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UBuildingUI* Building;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UTextBlock* Name;

public:
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UWrapBox* Top;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UVerticalBox* Content;

    UPROPERTY()
    ABuilding* Selected;

    UPROPERTY()
    TArray<UBuildingSelectedUIComponent*> Components;
    
    UBuildingSelectedUI* Init(ABuilding* actor);
    virtual void Tick() override;
};

UCLASS(Abstract)
class UBuildingSelectedUIComponent : public UObject {
    GENERATED_BODY()

public:    
    virtual void CreateUI(UBuildingSelectedUI* selectedUI) {}
    virtual void Tick(UBuildingSelectedUI* selectedUI) {}
};
