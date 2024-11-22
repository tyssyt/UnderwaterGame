// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TextBlock.h"
#include "XD/Buildings/BuildingUI.h"
#include "Components/ProgressBar.h"
#include "Components/WrapBox.h"
#include "XD/SelectedUI.h"
#include "XD/Resources/ResourceBalanceUI.h"
#include "ConstructionSiteUI.generated.h"

class UConstructionPlan;

UCLASS(Abstract)
class XD_API UConstructionSiteUI : public USelectedUI {
    GENERATED_BODY()

protected:
    UPROPERTY()
    UConstructionSite* ConstructionSite;
    
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UBuildingUI* Building;
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UTextBlock* BuildingName;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UWrapBox* Materials;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UProgressBar* Progress;
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UTextBlock* TimeRemaining;
    
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UResourceBalanceUI> ResourceBalanceUIClass;

public:
    UConstructionSiteUI* Init(UConstructionSite* constructionSite);
    virtual void Tick() override;
};
