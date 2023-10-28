// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Resource.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "ResourceUI.generated.h"

UCLASS(Abstract)
class XD_API UResourceUI : public UUserWidget {
    GENERATED_BODY()

protected:
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UImage* Image;

    UPROPERTY(BlueprintReadOnly)
    UResource* Resource;

public:
    UResourceUI* Init(UResource* resource);
};
