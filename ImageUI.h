// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "ImageUI.generated.h"

// only exist because UWidgetComponent needs a UUserWidget, so this is just a thin wrapper for a clickable Image
UCLASS(Abstract)
class XD_API UImageUI : public UUserWidget {
    GENERATED_BODY()
    
public:
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UImage* Image;
    
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UButton* Button;
};
