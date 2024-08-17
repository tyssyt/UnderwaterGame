// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BuilderMode.h"
#include "XD/Buildings/IndoorBuilding.h"
#include "IndoorBuilderMode.generated.h"

UCLASS()
class XD_API UIndoorBuilderMode : public UBuilderMode {
    GENERATED_BODY()

    bool Buildable = false;

    UPROPERTY()
    UConstructionPlan* ConstructionPlan;
    UPROPERTY()
    AIndoorBuilding* Preview;

    UPROPERTY()
    UBuilderModeExtensions* Extensions;

public:
    UIndoorBuilderMode* Init(UConstructionPlan* constructionPlan);

    virtual bool Tick(const ACameraPawn& camera) override;
    virtual UClass* IDK() override;
    virtual void Stop(bool cancelled) override;

private:
    void Position(const ACameraPawn& camera);

    void ConfirmPosition();
    void SetInvisible();
    void SetNotBuildable();
    void SetBuildable();

    void Rotate();
};
