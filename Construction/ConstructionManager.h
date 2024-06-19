// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BuilderShip.h"
#include "BuilderTaskQueue.h"
#include "ConstructionResources.h"
#include "XD/Buildings/PickupPad.h"

#include "Tickable.h"
#include "ConstructionManager.generated.h"

UCLASS()
class XD_API UConstructionManager : public UObject, public FTickableGameObject {
    GENERATED_BODY()

protected:
    UPROPERTY()
    TArray<ABuilderShip*> IdleBuilders;
    UPROPERTY()
    TArray<APickupPad*> PickupPads;
    UPROPERTY()
    UBuilderTaskQueue* Tasks;

    uint32 LastFrameNumberWeTicked = INDEX_NONE;

public:
    UConstructionManager();

    UPROPERTY()
    UConstructionResources* ConstructionResources;

    void AddPickupPad(APickupPad* pickupPad) { PickupPads.Add(pickupPad); }
    void RemovePickupPad(APickupPad* pickupPad) { PickupPads.Remove(pickupPad); }
    APickupPad* GetNearestPickupPad(const FVector& location);

    void AddIdleBuilder(ABuilderShip* builder) { IdleBuilders.Add(builder); }
    void AddTask(UBuilderTask* task) const { Tasks->Enqueue(task); }

    virtual void Tick(float DeltaTime) override;
    virtual TStatId GetStatId() const override {
        RETURN_QUICK_DECLARE_CYCLE_STAT( UConstructionManager, STATGROUP_Tickables );
    }
};
