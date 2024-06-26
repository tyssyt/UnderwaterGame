// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Resource.h"
#include "ResourceUI.h"
#include "Blueprint/UserWidget.h"
#include "Components/Border.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextBlock.h"
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
    UResourceUI* Resource;

public:
    void SetHave(int have) const;
    void SetNeed(int need, UResource* resource);
    int GetNeed() const;
};
