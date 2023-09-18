// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextBlock.h"
#include "PowerUI.generated.h"

UCLASS(Abstract)
class XD_API UPowerUI : public UUserWidget {
    GENERATED_BODY()

protected:
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UTextBlock* Production;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UTextBlock* Consumption;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UTextBlock* Balance;

public:    
    void Set(int production, int consumption) const;
    FSlateColor GetProductionColor() const;
    FSlateColor GetConsumptionColor() const;
};
