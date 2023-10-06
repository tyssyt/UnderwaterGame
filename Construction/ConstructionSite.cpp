// Fill out your copyright notice in the Description page of Project Settings.

#include "ConstructionSite.h"

#include "ConstructionManager.h"
#include "XD/Utils.h"
#include "XD/Buildings/Building.h"

ConstructionSite::ConstructionSite(AXActor* building, const UConstructionPlan* constructionPlan, FConstructionFlags flags)
        : ConstructionSite(building, constructionPlan->Time, constructionPlan->Materials, flags) {}

ConstructionSite::ConstructionSite(AXActor* building, int time, const TArray<Material>& materials, FConstructionFlags flags)
        : Building(building), Time(time), Materials(materials), Flags(flags) {
    Building->SetActorTickEnabled(false);
    if (ABuilding* bbuilding = Cast<ABuilding>(building))
        bbuilding->constructionState = EConstructionState::ConstructionSite;
}

ConstructionSite::~ConstructionSite() {}

void ConstructionSite::SetGhostMaterial(UMaterial* ghostMaterial) const {
    Building->SetAllMaterials(ghostMaterial);
}

void ConstructionSite::BeginConstruction() {
    // for now, construction is instant so we complete it here
    Building->SetAllMaterials(nullptr);
    Building->SetActorTickEnabled(true);

    if (ABuilding* building = Cast<ABuilding>(Building))
        building->OnConstructionComplete(Flags);

    The::ConstructionManager(Building)->FinishConstruction(this);
}

TPair<APickupPad*, Material> ConstructionSite::GetNextDelivery(TArray<ConstructionResource>& constructionResources) const {
    for (auto& neededMaterial : Materials) {
        int needed = neededMaterial.amount;
        if (const auto delivered = Material::Find(DeliveredMaterial, neededMaterial.resource))
            needed -= delivered->amount;
        if (needed <= 0)
            continue;

        for (auto& constructionResource : constructionResources)
            if (neededMaterial.resource == constructionResource.Resource)
                // TODO there is place for many optimization here, like preferring a resource that can be delivered in one trip or picking the nearest pad!
                return MakeTuple(constructionResource.Pads.Last().Value, Material(needed, neededMaterial.resource));
    }
    // no delivery found
    return MakeTuple(nullptr, Material(0, nullptr));
}

void ConstructionSite::DeliverMaterial(Material material) {
    for (auto& mat : DeliveredMaterial) {
        if (mat.resource == material.resource) {
            mat.amount += material.amount;
            return;
        }
    }

    DeliveredMaterial.Add(material);
}
