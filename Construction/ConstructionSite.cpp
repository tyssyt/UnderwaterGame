// Fill out your copyright notice in the Description page of Project Settings.

#include "ConstructionSite.h"

#include "ConstructionManager.h"
#include "The.h"

UUnderConstruction::UUnderConstruction() {
    const static ConstructorHelpers::FObjectFinder<UMaterial> GhostMaterialFinder(TEXT("/Game/Assets/Materials/GhostMaterials/GhostMaterial"));
    Material = GhostMaterialFinder.Object;
    Type = EType::NonInteractable;
    // TODO construction UI
}

UConstructionSite* UConstructionSite::Init(AXActor* building, const UConstructionPlan* constructionPlan, UBuilderModeExtensions* extensions) {
    return Init(building, constructionPlan->Time, constructionPlan->Materials, extensions);
}

UConstructionSite* UConstructionSite::Init(AXActor* building, int time, const TArray<Material>& materials, UBuilderModeExtensions* extensions) {
    Building = building;
    Time = time;
    Materials = materials;
    Extensions = extensions;

    if (ABuilding* bbuilding = Cast<ABuilding>(building)) {
        Condition = NewObject<UUnderConstruction>(this);
        bbuilding->AddCondition(Condition);
    } else // TODO make everything that is buildable extend ABuilding
        Building->SetActorTickEnabled(false);
    return this;
}

void UConstructionSite::BeginConstruction() {
    // for now, construction is instant so we complete it here
    if (const auto building = Cast<ABuilding>(Building)) {
        building->RemoveCondition(Condition);
        building->OnConstructionComplete(Extensions);
    } else // TODO make everything that is buildable extend ABuilding
        Building->SetActorTickEnabled(true);

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
