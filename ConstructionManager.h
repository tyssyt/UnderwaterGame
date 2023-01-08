// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BuilderShip.h"
#include "ConstructionSite.h"

#include <vector>

#include "ConstructionManager.generated.h"

UCLASS()
class XD_API UConstructionManager : public UObject {
    GENERATED_BODY()

public:
    UConstructionManager();
    ~UConstructionManager();

    void AddIdleBuilder(ABuilderShip* builder);
    void AddConstruction(AActor* building);
    void FinishConstruction(ConstructionSite* site);

    UPROPERTY(EditAnywhere)
    UMaterial* GhostMaterial;

private:
    std::vector<ABuilderShip*> idleBuilders;

    std::vector<ConstructionSite*> newConstructionSites;
    std::vector<ConstructionSite*> wipContructionSites; // TODO ununsed, but I think I want to have it?

};
