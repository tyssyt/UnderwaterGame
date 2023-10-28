// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ResourceUI.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "ResourceAmountUI.generated.h"

UCLASS(Abstract)
class XD_API UResourceAmountUI : public UUserWidget {
    GENERATED_BODY()

protected:
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UTextBlock* Count;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UResourceUI* Resource;

public:
    UResourceAmountUI* Init(int amount, UResource* resource);

};
