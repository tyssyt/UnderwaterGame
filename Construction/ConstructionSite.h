// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BuilderShip.h"
#include "XD/Buildings/Building.h"
#include "ConstructionSite.generated.h"

class UDeliverResource;

UCLASS()
class UUnderConstruction : public UCondition {
    GENERATED_BODY()
public:
    UUnderConstruction();
};

UCLASS()
class XD_API UConstructionSite : public UObject {
    GENERATED_BODY()

protected:
    UPROPERTY()
    UBuilderModeExtensions* Extensions;
    UPROPERTY()
    UUnderConstruction* Condition;

public:
    UPROPERTY()
    ABuilding* Building;
    TArray<Material> Materials;
    UPROPERTY()
    int Time;
    UPROPERTY()
    TArray<UDeliverResource*> Tasks;

    UConstructionSite* Init(ABuilding* building, const UConstructionPlan* constructionPlan, UBuilderModeExtensions* extensions); 
    UConstructionSite* Init(ABuilding* building, int time, const TArray<Material>& materials, UBuilderModeExtensions* extensions);

    void QueueTasks();
    void FinishConstruction() const;
};

UCLASS()
class UDeliverResource : public UBuilderTask {
    GENERATED_BODY()

    UPROPERTY()
    UConstructionSite* ConstructionSite;

    void PickupMaterial() const;

public:
    enum class EState { Collect, Deliver, Construct, Done } NextCommand;

    UDeliverResource* Init(UConstructionSite* constructionSite, const Material& material);    
    virtual ABuilderShip::FCommand GetNextCommand() override;    
};
