// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ResourceAmountUI.h"
#include "ResourceUI.h"
#include "Blueprint/UserWidget.h"
#include "ResourceRatioUI.generated.h"

UCLASS(Abstract)
class XD_API UResourceRatioUI : public UUserWidget {
    GENERATED_BODY()

protected:
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UResourceAmountUI* Numerator;
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UResourceAmountUI* Denominator;

public:
    UResourceRatioUI* Init(int numeratorAmount, UResource* numeratorResource, int denominatorAmount, UResource* denominatorResource);
};
