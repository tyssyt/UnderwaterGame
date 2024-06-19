// Fill out your copyright notice in the Description page of Project Settings.

#include "ScalingWidgetComponent.h"

#include "The.h"
#include "XD/CameraPawn.h"


UScalingWidgetComponent::UScalingWidgetComponent() {
    PrimaryComponentTick.bCanEverTick = true;
    SetWidgetSpace(EWidgetSpace::Screen);
    SetPivot(FVector2D(0.5, 0.5));
    SetGenerateOverlapEvents(false);
}

UScalingWidgetComponent* UScalingWidgetComponent::Init(
    UUserWidget* widget,
    float fullSizeDistance,
    float cutoffScale,
    TOptional<FVector2d> size
) {
    FullSizeDistance = fullSizeDistance;
    CutoffScale = cutoffScale;

    SetWidget(widget);
    if (size) {
        SetDrawSize(*size);
    } else {
        widget->ForceLayoutPrepass();
        SetDrawSize(widget->GetDesiredSize());
    }

    return this;
}

void UScalingWidgetComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    const double distance = FVector::Distance(The::CameraPawn(this)->GetActorLocation(), GetComponentLocation());
    const double scale = FMath::Min(FullSizeDistance / distance, 1.);

    if (scale < CutoffScale) {
        SetVisibility(false);
        return;
    }

    SetVisibility(true);
    GetWidget()->SetRenderScale(FVector2d(scale));
}
