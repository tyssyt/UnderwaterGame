// Fill out your copyright notice in the Description page of Project Settings.

#include "Building.h"

#include "ConstructionPlan.h"
#include "XD/ComponentX.h"
#include "XD/Construction/BuilderModeExtension.h"

ABuilding::ABuilding() : constructionState(EConstructionState::BuilderMode) {}

ABuilding* ABuilding::Init(UConstructionPlan* constructionPlan) {    
    for (const auto componentLoader : constructionPlan->ComponentLoaders)
        componentLoader->AddComponentToBuilding(this);
    return this;
}

void ABuilding::OnConstructionComplete(UBuilderModeExtensions* extensions) {
    constructionState = EConstructionState::Done;

    TInlineComponentArray<UComponentX*> components;
    GetComponents<>(components);
    for (const auto component : components)
        if (const auto extension = extensions->ComponentExtensions.Find(component))
            component->OnConstructionComplete(*extension);
        else
            component->OnConstructionComplete(nullptr);
}

UBuilderModeExtensions* ABuilding::CreateBuilderModeExtension() {
    const auto extensions = NewObject<UBuilderModeExtensions>(this);
    TInlineComponentArray<UComponentX*> components;
    GetComponents<>(components);
    for (const auto component : components)
        if (const auto extension = component->CreateBuilderModeExtension())
            extensions->ComponentExtensions.Add(component, extension);
    return extensions;
}

void ABuilding::InitSelectedUI(TArray<UBuildingSelectedUIComponent*>& components) {
    for (const auto component : GetInstanceComponents()) 
        if (const auto componentX = Cast<UComponentX>(component))
            componentX->AddToSelectedUI(components);
}
