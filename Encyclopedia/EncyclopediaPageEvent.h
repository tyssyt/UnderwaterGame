// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EncyclopediaPage.h"
#include "Components/RichTextBlock.h"
#include "Components/VerticalBox.h"
#include "XD/Map/Event/Event.h"
#include "EncyclopediaPageEvent.generated.h"

class UEventUI;
class UEncyclopedia;

UCLASS(Abstract)
class XD_API UEncyclopediaPageEvent : public UEncyclopediaPage {
    GENERATED_BODY()

protected:
    FSlateFontInfo LocationFont;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    URichTextBlock* Description;    

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UVerticalBox* Rewards;
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UVerticalBox* Location;
    
    void AddLocation(UEventInstance* EventInstance) const;

public:
    UEncyclopediaPageEvent* Init(UEvent* Event);
};
