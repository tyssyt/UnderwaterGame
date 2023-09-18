// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "XD/Buildings/ConstructionPlan.h"
#include "XD/Buildings/PickupPad.h"
#include "GameFramework/Actor.h"

class XD_API ConstructionSite {
public:
    ConstructionSite(AXActor* building, const UConstructionPlan* constructionPlan, FConstructionFlags flags); 
    ConstructionSite(AXActor* building, int time, const std::vector<Material>& materials, FConstructionFlags flags);
    ~ConstructionSite();
    
    void SetGhostMaterial(UMaterial* ghostMaterial) const;
    void BeginConstruction() const;

    AXActor* Building;
    int Time;    
    std::vector<Material> Materials;
    //class ABuilderShip* BuilderShip;

    std::pair<APickupPad*, Material> GetNextDelivery(std::vector<struct ConstructionResource>& constructionResources) const;
    void DeliverMaterial(Material material);

private:
    std::vector<Material> DeliveredMaterial;
    FConstructionFlags Flags;

};
