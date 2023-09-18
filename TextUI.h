// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "TextUI.generated.h"

// only exist because UWidgetComponent needs a UUserWidget, so this is just a thin wrapper
UCLASS(Abstract)
class XD_API UTextUI : public UUserWidget {
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UTextBlock* Text;
};
