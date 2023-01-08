// Fill out your copyright notice in the Description page of Project Settings.

#include "ConstructionManager.h"

#include <algorithm>


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

// TODO if Tick can happen multi-threaded, these calls need to be synchronized

void UConstructionManager::AddIdleBuilder(ABuilderShip* builder) {
    if (!newConstructionSites.empty()) {
        ConstructionSite* site = newConstructionSites.back();
        newConstructionSites.pop_back();

        wipContructionSites.push_back(site);
        builder->StartConstructing(site);
    } else {
        idleBuilders.push_back(builder);
    }
}

void UConstructionManager::AddConstruction(AActor* building) {
    ConstructionSite* constructionSite = new ConstructionSite(building, GhostMaterial);
    if (!idleBuilders.empty()) {
        ABuilderShip* builder = idleBuilders.back();
        idleBuilders.pop_back();

        wipContructionSites.push_back(constructionSite);
        builder->StartConstructing(constructionSite);
    } else {
        newConstructionSites.push_back(constructionSite);
    }
}

void UConstructionManager::FinishConstruction(ConstructionSite* constructionSite) {
    wipContructionSites.erase(std::remove(wipContructionSites.begin(), wipContructionSites.end(), constructionSite), wipContructionSites.end());
    delete constructionSite;
}
