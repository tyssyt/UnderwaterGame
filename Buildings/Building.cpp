// Fill out your copyright notice in the Description page of Project Settings.

#include "Building.h"

#include "ConstructionPlan.h"
#include "Components/BillboardComponent.h"
#include "XD/ComponentX.h"
#include "XD/Construction/BuilderModeExtension.h"

bool UCondition::DisablesTick() const {
    switch (Type) {
    case EType::NonInteractable: case EType::TickDisabled:
        return true;
    case EType::Visual:
        return false;
    default:
        checkNoEntry();
        return false;
    }
}

ABuilding* ABuilding::Init(UConstructionPlan* constructionPlan) {    
    for (const auto componentLoader : constructionPlan->ComponentLoaders)
        componentLoader->AddComponentToBuilding(this);
    return this;
}

void ABuilding::OnConstructionComplete(UBuilderModeExtensions* extensions) {
    TInlineComponentArray<UComponentX*> components;
    GetComponents<>(components);
    for (const auto component : components)
        if (const auto extension = extensions->ComponentExtensions.Find(component))
            component->OnConstructionComplete(*extension);
        else
            component->OnConstructionComplete(nullptr);
}

void ABuilding::AddCondition(UCondition* condition) {
    Conditions.Add(condition);
    if (condition->GetSymbol()) {
        auto floatingSymbol = GetComponentByClass<UBillboardComponent>();
        check(!floatingSymbol); // TODO handle multiple symbols

        floatingSymbol = NewObject<UBillboardComponent>(this, TEXT("ConditionBillboard"));
        floatingSymbol->RegisterComponent();
        floatingSymbol->AttachToComponent(GetRootComponent(), FAttachmentTransformRules(EAttachmentRule::KeepRelative, true));
        floatingSymbol->SetSprite(condition->GetSymbol()); 
        floatingSymbol->SetRelativeLocation(FVector(.0f, .0f, 60.f));
        floatingSymbol->SetHiddenInGame(false);
        floatingSymbol->SetVisibility(true);
        AddInstanceComponent(floatingSymbol);
    }
    if (condition->GetMaterial())
        SetAllMaterials(condition->GetMaterial());
    if (condition->DisablesTick())
        SetActorTickEnabled(false);
}
void ABuilding::RemoveCondition(UCondition* condition) {
    const int removed = Conditions.Remove(condition);
    if (removed <= 0)
        return;

    if (condition->GetSymbol()) {
        // TODO handle multiple symbols
        const auto floatingSymbol = GetComponentByClass<UBillboardComponent>();
        RemoveInstanceComponent(floatingSymbol);
        floatingSymbol->DestroyComponent();
    }
    if (condition->GetMaterial()) {
        UMaterialInterface* newMat = nullptr;
        for (int i = Conditions.Num()-1; i >= 0; i--) {
            if (Conditions[i]->GetMaterial()) {
                newMat = Conditions[i]->GetMaterial();
                break;
            }
        }
        SetAllMaterials(newMat);
    }
    if (condition->DisablesTick()) {
        bool enable = true;
        for (const auto c : Conditions) {
            if (c->DisablesTick()) {
                enable = false;
                break;
            }
        }
        SetActorTickEnabled(enable);
    }
}

UCondition* ABuilding::IsNonInteractable() const {
    for (const auto condition : Conditions)
        if (condition->GetType() == UCondition::EType::NonInteractable)
            return condition;
    return nullptr;
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
