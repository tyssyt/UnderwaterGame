// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NeedUI.h"
#include "Blueprint/UserWidget.h"
#include "Components/HorizontalBox.h"
#include "XD/Buildings/BuildingUI.h"
#include "XD/Resources/ResourceRatioUI.h"
#include "NeedSatisfierUI.generated.h"

class UNeedSatisfier;

UCLASS(Abstract)
class XD_API UNeedSatisfierUI : public UUserWidget {
    GENERATED_BODY()

protected:
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UResourceRatioUI> ResourceRatioUIClass;
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UBuildingUI> BuildingUIClass;

public:
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UHorizontalBox* Satisfiers;
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UNeedUI* Need;

    UNeedSatisfierUI* Init(UNeedSatisfier* satisfier);
};
