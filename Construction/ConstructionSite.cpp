// Fill out your copyright notice in the Description page of Project Settings.

#include "ConstructionSite.h"

#include "ConstructionManager.h"
#include "XD/Buildings/Building.h"


ConstructionSite::ConstructionSite(AXActor* building, UConstructionPlan* constructionPlan) : ConstructionSite(building, constructionPlan->Time, constructionPlan->Materials) {}

ConstructionSite::ConstructionSite(AXActor* building, int time, std::vector<Material> materials) : Building(building), Time(time), Materials(materials) {
    Building->SetActorTickEnabled(false);
    ABuilding* bbuilding = Cast<ABuilding>(building); // TODO eventually we want the input of this function to be some kind of common class...
    if (bbuilding) {
        bbuilding->constructionState = ConstructionState::ConstructionSite;
    }
}

ConstructionSite::~ConstructionSite() {}

void ConstructionSite::SetGhostMaterial(UMaterial* ghostMaterial) const {
    Building->SetAllMaterials(ghostMaterial);
}

void ConstructionSite::BeginConstruction() const {
    // for now, construction is instant so we complete it here
    Building->SetAllMaterials(nullptr);
    Building->SetActorTickEnabled(true);
    
    ABuilding* building = Cast<ABuilding>(Building); // TODO eventually we want the input of this function to be some kind of common class...
    if (building) {
        building->OnConstructionComplete();
    }
}

std::pair<APickupPad*, Material> ConstructionSite::GetNextDelivery(std::vector<ConstructionResource>* constructionResources) const {
    for (auto& neededMaterial : Materials) {
        int needed = neededMaterial.amount;

        // subtract already delivered material
        for (auto& deliveredMaterial : DeliveredMaterial) {
            if (neededMaterial.resource == deliveredMaterial.resource) {
                needed -= deliveredMaterial.amount;
            }
        }

        if (needed <= 0)
            continue;

        for (auto& constructionResource : *constructionResources) {
            if (neededMaterial.resource == constructionResource.Resource) {
                // TODO there is place for many optimization here, like preferring a resource that can be delivered in one trip or picking the nearest pad!
                return std::make_pair(constructionResource.Pads.back().second,Material(needed, neededMaterial.resource));
            }
        }
    }
    // no delivery found
    return std::make_pair(nullptr, Material(0, nullptr));
}

void ConstructionSite::DeliverMaterial(Material material) {
    for (auto& mat : DeliveredMaterial) {
        if (mat.resource == material.resource) {
            mat.amount += material.amount;
            return;
        }
    }

    DeliveredMaterial.push_back(material);
}
