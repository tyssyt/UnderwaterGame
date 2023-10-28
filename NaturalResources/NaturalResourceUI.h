// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NaturalResource.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "NaturalResourceUI.generated.h"

UCLASS(Abstract)
class XD_API UNaturalResourceUI : public UUserWidget {
    GENERATED_BODY()

protected:
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UImage* Image;

    UPROPERTY(BlueprintReadOnly)
    UNaturalResource* NaturalResource;

public:
    UNaturalResourceUI* Init(UNaturalResource* naturalResource);
};
