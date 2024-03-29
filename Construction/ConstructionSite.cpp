// Fill out your copyright notice in the Description page of Project Settings.

#include "ConstructionSite.h"

#include "ConstructionManager.h"
#include "The.h"

UConstructionSite* UConstructionSite::Init(AXActor* building, const UConstructionPlan* constructionPlan, UConstructionOptions* options) {
    return Init(building, constructionPlan->Time, constructionPlan->Materials, options);
}

UConstructionSite* UConstructionSite::Init(AXActor* building, int time, const TArray<Material>& materials, UConstructionOptions* options) {
    Building = building;
    Time = time;
    Materials = materials;
    ConstructionOptions = options;

    Building->SetActorTickEnabled(false);
    if (ABuilding* bbuilding = Cast<ABuilding>(building))
        bbuilding->constructionState = EConstructionState::ConstructionSite;
    return this;
}

void UConstructionSite::SetGhostMaterial(UMaterial* ghostMaterial) const {
    Building->SetAllMaterials(ghostMaterial);
}

void UConstructionSite::BeginConstruction() {
    // for now, construction is instant so we complete it here
    Building->SetAllMaterials(nullptr);
    Building->SetActorTickEnabled(true);

    if (ABuilding* building = Cast<ABuilding>(Building))
        building->OnConstructionComplete(ConstructionOptions);

    The::ConstructionManager(Building)->FinishConstruction(this);
}

TPair<APickupPad*, Material> UConstructionSite::GetNextDelivery(TArray<ConstructionResource>& constructionResources) const {
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

void UConstructionSite::DeliverMaterial(Material material) {
    for (auto& mat : DeliveredMaterial) {
        if (mat.resource == material.resource) {
            mat.amount += material.amount;
            return;
        }
    }

    DeliveredMaterial.Add(material);
}
