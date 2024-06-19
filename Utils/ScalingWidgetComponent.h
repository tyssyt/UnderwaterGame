// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "ScalingWidgetComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class XD_API UScalingWidgetComponent : public UWidgetComponent {
    GENERATED_BODY()

protected:
    UPROPERTY()
    float FullSizeDistance;
    UPROPERTY()
    float CutoffScale;

public:
    UScalingWidgetComponent();
    UScalingWidgetComponent* Init(UUserWidget* widget, float fullSizeDistance, float cutoffScale, TOptional<FVector2d> size = {});    
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
