// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EncyclopediaPage.h"
#include "Components/RichTextBlock.h"
#include "EncyclopediaPageText.generated.h"

class UEncyclopedia;

UCLASS(Abstract)
class XD_API UEncyclopediaPageText : public UEncyclopediaPage {
    GENERATED_BODY()

protected:
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    URichTextBlock* Description;

public:
    UEncyclopediaPageText* Init(const FText& text);
};
