// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "XActor.h"
#include "GameFramework/Actor.h"
#include "XD/GameInstanceX.h"
#include "Building.generated.h"


enum class ConstructionState { BuilderMode, ConstructionSite, Done };

UCLASS()
class XD_API ABuilding : public AXActor {
    GENERATED_BODY()

public:
    ABuilding();

    ConstructionState constructionState;

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

};
