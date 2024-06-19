// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/HorizontalBox.h"
#include "ConditionListUI.generated.h"

UCLASS(Abstract)
class XD_API UConditionListUI : public UUserWidget {
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UHorizontalBox* List;
};
