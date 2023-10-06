// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BuilderShip.h"
#include "ConstructionSite.h"
#include "XD/Buildings/PickupPad.h"

#include <vector>
#include <deque>

#include "Tickable.h"
#include "ConstructionManager.generated.h"


struct XD_API ConstructionResource {
    ConstructionResource(const UResource* resource);
    
    const UResource* const Resource;

    TArray<std::pair<int, APickupPad*>> Pads;
    int Total = 0;
    int Reserved = 0;
};

UCLASS()
class XD_API UConstructionManager : public UObject, public FTickableGameObject {
    GENERATED_BODY()

public:
    UConstructionManager();

    void SetConstructionResources(const TSet<UResource*>& constructionResources);

    UFUNCTION(BlueprintCallable)
    void AddIdleBuilder(ABuilderShip* builder);
    
    void AddConstruction(ConstructionSite* constructionSite);
    
    UFUNCTION(BlueprintCallable)
    void AddPickupPad(APickupPad* pickupPad);

    void UnreserveResource(UResource* resource, int amount);
    void FinishConstruction(const ConstructionSite* constructionSite);

    virtual void Tick(float DeltaTime) override;    
    virtual TStatId GetStatId() const override {
        RETURN_QUICK_DECLARE_CYCLE_STAT( FMyTickableThing, STATGROUP_Tickables );
    }

    UPROPERTY(EditAnywhere)
    UMaterial* GhostMaterial;
    
    TArray<ConstructionResource> ConstructionResources;

private:
    
    std::deque<ABuilderShip*> IdleBuilders;

    // TODO optimization: when there are a lot of Buildings waiting for resources, we can store them in Groups by building and so we only need to check once for each building type. Can become relevant if we have Blueprints and thousands of things need to be build
    std::deque<ConstructionSite*> newConstructionSites;
    std::vector<ConstructionSite*> wipConstructionSites; // TODO ununsed, but I think I want to have it?

    std::vector<APickupPad*> PickupPads;
    
    // The last frame number we were ticked.
    // We don't want to tick multiple times per frame 
    uint32 LastFrameNumberWeTicked = INDEX_NONE;

    ConstructionSite* FindBuildableConstructionSite();
    bool HasResourcesFor(const TArray<Material>* materials) const;
};
