// Fill out your copyright notice in the Description page of Project Settings.

#include "ConstructionManager.h"

#include <algorithm>


ConstructionResource::ConstructionResource(const UResource* resource) : Resource(resource) {}
ConstructionResource::~ConstructionResource() {}

UConstructionManager::UConstructionManager() {
    // Structure to hold one-time initialization
    struct FConstructorStatics {
        ConstructorHelpers::FObjectFinderOptional<UMaterial> GhostMaterial;
        FConstructorStatics() : GhostMaterial(TEXT("/Game/GhostMaterial")) {}
    };
    static FConstructorStatics ConstructorStatics;

    // Create static mesh component
    GhostMaterial = ConstructorStatics.GhostMaterial.Get();
}

UConstructionManager::~UConstructionManager() {}

UConstructionManager* UConstructionManager::Init(UResourceBook* theResourceBook) {
    constructionResources.emplace_back(theResourceBook->LargeParts);
    constructionResources.emplace_back(theResourceBook->Cable);
    constructionResources.emplace_back(theResourceBook->Rubber);
    return this;
}

// TODO if Tick can happen multi-threaded, these calls need to be synchronized

void UConstructionManager::AddIdleBuilder(ABuilderShip* builder) {
    idleBuilders.push_back(builder);
}

void UConstructionManager::AddConstruction(ConstructionSite* constructionSite) {
    constructionSite->SetGhostMaterial(GhostMaterial);
    newConstructionSites.push_back(constructionSite);
}

void UConstructionManager::AddPickupPad(APickupPad* pickupPad) {
    pickupPads.push_back(pickupPad);
}

void UConstructionManager::UnreserveResource(UResource* resource, int amount) {
    for (auto& constructionResource : constructionResources) {
        if (constructionResource.Resource == resource) {
            constructionResource.Reserved -= amount;
        }
    }
}

void UConstructionManager::FinishConstruction(ConstructionSite* constructionSite) {
    wipConstructionSites.erase(std::remove(wipConstructionSites.begin(), wipConstructionSites.end(), constructionSite), wipConstructionSites.end());
    delete constructionSite;
}

void UConstructionManager::Tick(float DeltaTime) {
    // TODO the LastFrameNumberWeTicked is copy pasted from the tutorial on ticking, should I put this in other places as well?
    if (LastFrameNumberWeTicked == GFrameCounter)
        return;
    LastFrameNumberWeTicked = GFrameCounter;

    // collect which resources are where
    for (auto& constructionResource : constructionResources) {
        constructionResource.Total = 0;
        constructionResource.Pads.clear();
    }
    
    for (APickupPad* pad : pickupPads) {
        for (auto& input : pad->Inventory->GetInputs()) {
            if (!input.Resource)
                continue;
            
            for (auto& constructionResource : constructionResources) {
                if (input.Resource == constructionResource.Resource) {
                    constructionResource.Total += input.Current;
                    constructionResource.Pads.emplace_back(input.Current, pad);
                }
            }
        }
    }

    // sort by the amount of resources
    for (auto& constructionResource : constructionResources) {
        std::sort(constructionResource.Pads.begin(), constructionResource.Pads.end());
    }

    // start construction
    while (!idleBuilders.empty() && !newConstructionSites.empty()) {
        ConstructionSite* constructionSite = FindBuildableConstructionSite();
        if (!constructionSite)
            return;
        
        // reserve resources
        for (auto& material : constructionSite->Materials) {
            for (auto& constructionResource : constructionResources) {
                if (material.resource == constructionResource.Resource) {
                    constructionResource.Reserved += material.amount;
                }
            }
        }

        // get Ship
        ABuilderShip* builder = idleBuilders.front();
        idleBuilders.pop_front();
    
        wipConstructionSites.push_back(constructionSite);
        builder->StartConstructing(constructionSite);
    }
}

ConstructionSite* UConstructionManager::FindBuildableConstructionSite() {
    for (auto it = newConstructionSites.begin(); it != newConstructionSites.end(); ++it) {
        ConstructionSite* cur = *it;

        if (!HasResourcesFor(&cur->Materials))
            continue;
        
        newConstructionSites.erase(it);
        return cur;
    }
    return nullptr;
}

bool UConstructionManager::HasResourcesFor(const std::vector<Material>* materials) const {    
    for (auto& material : *materials) {
        for (auto& constructionResource : constructionResources) {
            if (material.resource == constructionResource.Resource) {
                if (material.amount > constructionResource.Total - constructionResource.Reserved) {
                    return false;
                }
            }
        }
    }
    return true;
}

void UConstructionManager::UpdateUI(URessourcesUI* ui) const {
    UInventorySlotUI* slots[] { ui->InventorySlot_1, ui->InventorySlot_2, ui->InventorySlot_3 };
    for (int i=0; i<3; ++i) {
        const UResource* resource = constructionResources[i].Resource;

        int max = 0;
        for (auto& pad : constructionResources[i].Pads) {
            for (auto& input : pad.second->Inventory->GetInputs()) {
                if (input.Resource == resource) {
                    max += input.Max;
                }
            }
        }
        
        slots[i]->Set(constructionResources[i].Total - constructionResources[i].Reserved, max, resource);
    }
    
    ui->InventorySlot_4->Clear();
}
