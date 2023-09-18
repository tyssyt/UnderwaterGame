// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "XActor.h"
#include "Building.generated.h"

UENUM(BlueprintType)
enum class EConstructionState { BuilderMode, ConstructionSite, Done };

struct XD_API FConstructionFlags {
    bool autoConnectWires;
};

UCLASS()
class XD_API ABuilding : public AXActor {
    GENERATED_BODY()

public:
    ABuilding();

    UPROPERTY(EditAnywhere)
    EConstructionState constructionState;

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;
    virtual void OnConstructionComplete(FConstructionFlags flags); // TODO maybe there is some unreal lifecycle method we can hook into?

};
