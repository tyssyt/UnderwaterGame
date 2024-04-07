// Fill out your copyright notice in the Description page of Project Settings.

#include "Building.h"

#include "ConstructionPlan.h"
#include "XD/ComponentX.h"

ABuilding::ABuilding() : constructionState(EConstructionState::BuilderMode) {}

ABuilding* ABuilding::Init(UConstructionPlan* constructionPlan) {    
    for (const auto componentLoader : constructionPlan->ComponentLoaders)
        componentLoader->AddComponentToBuilding(this);
    return this;
}

void ABuilding::OnConstructionComplete(UConstructionOptions* options) {
    constructionState = EConstructionState::Done;

    TInlineComponentArray<UComponentX*> components;
    GetComponents<>(components);
    for (const auto component : components)
        component->OnConstructionComplete(options);
}

void ABuilding::InitSelectedUI(TArray<UBuildingSelectedUIComponent*>& components) {
    for (const auto component : GetInstanceComponents()) 
        if (const auto componentX = Cast<UComponentX>(component))
            componentX->AddToSelectedUI(components);
}

