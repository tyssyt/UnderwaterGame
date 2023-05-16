// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Buildings/ConstructionPlan.h"
#include "Buildings/PickupPad.h"
#include "GameFramework/Actor.h"

class XD_API ConstructionSite {
public:
    ConstructionSite(AActor* building, UConstructionPlan* constructionPlan); 
    ConstructionSite(AActor* building, int time, std::vector<Material> materials);
    ~ConstructionSite();
    
    void SetGhostMaterial(UMaterial* ghostMaterial) const;
    void BeginConstruction() const;

    AActor* Building;
    int Time;    
    std::vector<Material> Materials;
    //class ABuilderShip* BuilderShip;

    std::pair<APickupPad*, Material> GetNextDelivery(std::vector<struct ConstructionResource>* constructionResources) const;
    void DeliverMaterial(Material material);

private:
    std::vector<Material> DeliveredMaterial;

};
