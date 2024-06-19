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
    void SetProgress(float progress) const;
};

UCLASS()
class XD_API UConstructionSite : public UObject, public FTickableGameObject {
    GENERATED_BODY()

protected:
    uint32 LastFrameNumberWeTicked = INDEX_NONE;
    UPROPERTY()
    UBuilderModeExtensions* Extensions;
    UPROPERTY()
    UUnderConstruction* Condition;

    enum class EState { Initial, Constructing, Finished } State;

public:
    UPROPERTY()
    ABuilding* Building;
    TArray<Material> Materials;
    UPROPERTY()
    int Time;
    UPROPERTY()
    int Progress;
    UPROPERTY()
    TArray<UDeliverResource*> Tasks;

    UConstructionSite* Init(ABuilding* building, const UConstructionPlan* constructionPlan, UBuilderModeExtensions* extensions); 
    UConstructionSite* Init(ABuilding* building, int time, const TArray<Material>& materials, UBuilderModeExtensions* extensions);

    void QueueTasks();
    void StartConstruction();

    virtual void Tick(float DeltaTime) override;
    virtual TStatId GetStatId() const override {
        RETURN_QUICK_DECLARE_CYCLE_STAT( UConstructionSite, STATGROUP_Tickables );
    }
    virtual ETickableTickType GetTickableTickType() const override { return ETickableTickType::Always; }

protected:
    void FinishConstruction();
};

UCLASS()
class UDeliverResource : public UBuilderTask {
    GENERATED_BODY()

    UPROPERTY()
    UConstructionSite* ConstructionSite;

    void PickupMaterial() const;
    ABuilderShip::FCommand Deliver() const;
    ABuilderShip::FCommand Construct();

public:
    enum class EState { Collect, Deliver, Construct, Done } NextCommand;

    UDeliverResource* Init(UConstructionSite* constructionSite, const Material& material);    
    virtual ABuilderShip::FCommand GetNextCommand() override;    
};
