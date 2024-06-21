// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ArrowMover.h"
#include "BuilderMode.h"
#include "Components/WidgetComponent.h"
#include "XD/Buildings/ConstructionPlan.h"
#include "BuildingBuilderMode.generated.h"

class UBuilderModeExtensions;

UCLASS()
class XD_API UBuildingBuilderMode : public UBuilderMode {
    GENERATED_BODY()

    // TODO have some form of alignment, i.e. to Neighbors or a global grid

    bool HasOverlap = false;

    enum Phase { Positioning, Waiting, Done };

    Phase Phase = Positioning;
    
    UPROPERTY()
    UConstructionPlan* ConstructionPlan;
    UPROPERTY()
    ABuilding* Preview;

    UPROPERTY()
    UWidgetComponent* ConfirmSymbol;
    UPROPERTY()
    UWidgetComponent* CancelSymbol;
    UPROPERTY()
    UArrowMover* ArrowForward;
    UPROPERTY()
    UArrowMover* ArrowSideways;
    UPROPERTY()
    UArrowMover* ArrowRotate;

    UPROPERTY()
    UBuilderModeExtensions* Extensions;

    void TickPosition(const ACameraPawn& camera) const;
    void TickWait() const;
    void CheckOverlap();
    void ConfirmPosition();
    UFUNCTION()
    void OnClickConfirm();
    UFUNCTION()
    void OnClickCancel();
    
    void RemoveBindingsWaiting() const;

    void PushUp();
    void PushDown();
    void PushLeft();
    void PushRight();
    void RotateLeft();
    void RotateRight();
    void Rotate90();

public:
    UBuildingBuilderMode* Init(UConstructionPlan* constructionPlan);
    virtual bool Tick(const ACameraPawn& camera) override;
    virtual UClass* IDK() override;
    virtual void Stop(bool cancelled) override;
};
