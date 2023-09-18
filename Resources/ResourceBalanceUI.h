// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Resource.h"
#include "Blueprint/UserWidget.h"
#include "Components/Border.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "ResourceBalanceUI.generated.h"

UCLASS(Abstract)
class XD_API UResourceBalanceUI : public UUserWidget {
    GENERATED_BODY()

    int NeedInt;

protected:
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UTextBlock* Need;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UTextBlock* Have;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UBorder* Border;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UImage* ResourceImage;

public:
    void SetHave(int have) const;
    void SetNeed(int need, const UResource* resource);
};
