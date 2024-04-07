// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EncyclopediaPage.h"
#include "Components/Border.h"
#include "Components/RichTextBlock.h"
#include "Components/VerticalBox.h"
#include "XD/PopulationManager/NeedSatisfierUI.h"
#include "EncyclopediaPageNeed.generated.h"

class UEncyclopedia;

UCLASS(Abstract)
class XD_API UEncyclopediaPageNeed : public UEncyclopediaPage {
    GENERATED_BODY()

protected:
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    URichTextBlock* Description;
    
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UResourceAmountUI* InterestStart;
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UResourceAmountUI* FullNeed;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UVerticalBox* SatisfiedBy;
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UBorder* SatisfierBox;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UNeedSatisfierUI> NeedSatisfierUIClass;

public:
    UEncyclopediaPageNeed* Init(UNeed* need, UEncyclopedia* encyclopedia);
};
