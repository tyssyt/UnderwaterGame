// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Need.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "NeedUI.generated.h"

UCLASS(Abstract)
class XD_API UNeedUI : public UUserWidget {
    GENERATED_BODY()

protected:
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UImage* Image;

public:
    UPROPERTY(BlueprintReadOnly)
    UNeed* Need;

    UNeedUI* Init(UNeed* need);
};
