// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BuilderMode.h"
#include "DismantleMode.generated.h"

UCLASS()
class XD_API UDismantleMode : public UBuilderMode {
    GENERATED_BODY()

    UPROPERTY()
    ABuilding* Highlight;
    UPROPERTY()
    bool HighlightValid;

    UPROPERTY()
    bool Done;

public:
    UDismantleMode* Init();
    virtual UClass* IDK() override { return StaticClass(); } 
    virtual bool Tick(const ACameraPawn& camera) override;
    virtual void Stop(bool cancelled) override;

protected:
    void Confirm();
    void SetHighlight(ABuilding* building);
    bool CanBeDismantled(ABuilding* building);
};
