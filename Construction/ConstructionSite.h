// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "XD/Buildings/ConstructionPlan.h"
#include "XD/Buildings/PickupPad.h"
#include "GameFramework/Actor.h"

class XD_API ConstructionSite {
public:
    ConstructionSite(AXActor* building, const UConstructionPlan* constructionPlan, FConstructionFlags flags); 
    ConstructionSite(AXActor* building, int time, const TArray<Material>& materials, FConstructionFlags flags);
    ~ConstructionSite();
    
    void SetGhostMaterial(UMaterial* ghostMaterial) const;
    void BeginConstruction();

    AXActor* Building;
    int Time;    
    TArray<Material> Materials;
    //class ABuilderShip* BuilderShip;

    TPair<APickupPad*, Material> GetNextDelivery(TArray<struct ConstructionResource>& constructionResources) const;
    void DeliverMaterial(Material material);

private:
    TArray<Material> DeliveredMaterial;
    FConstructionFlags Flags;

};
