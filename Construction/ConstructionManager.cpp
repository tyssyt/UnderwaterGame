// Fill out your copyright notice in the Description page of Project Settings.

#include "ConstructionManager.h"

#include "XD/Cheats.h"

ConstructionResource::ConstructionResource(const UResource* resource) : Resource(resource) {}

UConstructionManager::UConstructionManager() {
    const static ConstructorHelpers::FObjectFinder<UMaterial> GhostMaterialFinder(TEXT("/Game/Assets/Materials/GhostMaterials/GhostMaterial"));
    GhostMaterial = GhostMaterialFinder.Object;
}


void UConstructionManager::SetConstructionResources(const TSet<UResource*>& constructionResources) {
    for (const UResource* resource : constructionResources)
        ConstructionResources.Add(resource);
}

// TODO if Tick can happen multi-threaded, these calls need to be synchronized
void UConstructionManager::AddIdleBuilder(ABuilderShip* builder) {
    IdleBuilders.push_back(builder);
}

void UConstructionManager::AddConstruction(UConstructionSite* constructionSite) {
    if (Cheats::INSTA_BUILD) {
        constructionSite->BeginConstruction();
    } else {
        constructionSite->SetGhostMaterial(GhostMaterial);
        NewConstructionSites.Add(constructionSite);
    }
}

void UConstructionManager::AddPickupPad(APickupPad* pickupPad) {
    PickupPads.Add(pickupPad);
}

void UConstructionManager::UnreserveResource(UResource* resource, int amount) {
    for (auto& constructionResource : ConstructionResources)
        if (constructionResource.Resource == resource)
            constructionResource.Reserved -= amount;
}

void UConstructionManager::FinishConstruction(UConstructionSite* constructionSite) {
    WipConstructionSites.Remove(constructionSite);
}

void UConstructionManager::Tick(float DeltaTime) {
    // TODO the LastFrameNumberWeTicked is copy pasted from the tutorial on ticking, should I put this in other places as well?
    if (LastFrameNumberWeTicked == GFrameCounter)
        return;
    LastFrameNumberWeTicked = GFrameCounter;

    // collect which resources are where
    for (auto& constructionResource : ConstructionResources) {
        constructionResource.Total = 0;
        constructionResource.Pads.Empty();
    }
    
    for (APickupPad* pad : PickupPads) {
        for (auto& input : pad->Inventory->GetInputs()) {
            if (!input.Resource)
                continue;
            
            for (auto& constructionResource : ConstructionResources) {
                if (input.Resource == constructionResource.Resource) {
                    constructionResource.Total += input.Current;
                    constructionResource.Pads.Emplace(input.Current, pad);
                }
            }
        }
    }

    // sort by the amount of resources
    for (auto& constructionResource : ConstructionResources)
        constructionResource.Pads.Sort();

    // start construction
    while (!IdleBuilders.empty() && !NewConstructionSites.IsEmpty()) {
        const auto constructionSite = FindBuildableConstructionSite();
        if (!constructionSite)
            return;
        
        // reserve resources
        for (const auto& material : constructionSite->Materials) {
            for (auto& constructionResource : ConstructionResources) {
                if (material.resource == constructionResource.Resource) {
                    constructionResource.Reserved += material.amount;
                }
            }
        }

        // get Ship
        ABuilderShip* builder = IdleBuilders.front();
        IdleBuilders.pop_front();
    
        WipConstructionSites.Add(constructionSite);
        builder->StartConstructing(constructionSite);
    }
}

UConstructionSite* UConstructionManager::FindBuildableConstructionSite() {
    const int idx = NewConstructionSites.IndexOfByPredicate([this](const UConstructionSite* site) {
        return HasResourcesFor(&site->Materials);
    });
    if (idx == INDEX_NONE)
        return nullptr;

    const auto site = NewConstructionSites[idx];
    NewConstructionSites.RemoveAt(idx); // TODO optimization if NewConstructionSites is a queue this should be a little faster?
    return site;
}

bool UConstructionManager::HasResourcesFor(const TArray<Material>* materials) const {    
    for (auto& material : *materials)
        for (auto& constructionResource : ConstructionResources)
            if (material.resource == constructionResource.Resource)
                if (material.amount > constructionResource.Total - constructionResource.Reserved)
                    return false;
    return true;
}

