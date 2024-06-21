// Fill out your copyright notice in the Description page of Project Settings.

#include "Building.h"

#include "ConditionListUI.h"
#include "ConstructionPlan.h"
#include "ScalingWidgetComponent.h"
#include "The.h"
#include "Blueprint/WidgetTree.h"
#include "Components/HorizontalBoxSlot.h"
#include "XD/BlueprintHolder.h"
#include "XD/ComponentX.h"
#include "XD/PlayerControllerX.h"
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
void ABuilding::OnDismantleStart() {    
    TInlineComponentArray<UComponentX*> components;
    GetComponents<>(components);
    for (const auto component : components)
        component->OnDismantle();
}

void ABuilding::OnDismantleFinish() {
    const auto constructionPlan = The::Encyclopedia(this)->GetBuilding(GetClass());
    if (const auto naturalResource = constructionPlan->ConstructedOn)
        naturalResource->Respawn(this);
}

void ABuilding::AddCondition(UCondition* condition) {
    check(condition->GetSource());
    Conditions.Add(condition);
    if (const auto symbol = condition->GetSymbol()) {
        auto floatingSymbols = GetComponentByClass<UScalingWidgetComponent>();
        if (!floatingSymbols) {
            const auto playerController = The::PlayerController(this);
            const auto conditionList = CreateWidget<UConditionListUI>(playerController, playerController->BlueprintHolder->ConditionListUIClass);
            floatingSymbols = NewObject<UScalingWidgetComponent>(this, TEXT("ConditionsWidget"));
            floatingSymbols->Init(conditionList, 500., 1/4.);
            floatingSymbols->RegisterComponent();
            floatingSymbols->AttachToComponent(GetRootComponent(), FAttachmentTransformRules(EAttachmentRule::KeepRelative, true));
            floatingSymbols->SetRelativeLocation(FVector(0,0,60.));
            AddInstanceComponent(floatingSymbols);
        }

        const auto widget = Cast<UConditionListUI>(floatingSymbols->GetWidget());
        check(widget);

        condition->Image = widget->WidgetTree->ConstructWidget<UImage>();
        condition->Image->SetBrushFromTexture(symbol);
        condition->Image->SetDesiredSizeOverride(FVector2d(240, 240));

        const auto slot = widget->List->AddChildToHorizontalBox(condition->Image);
        slot->SetHorizontalAlignment(HAlign_Center);
        slot->SetVerticalAlignment(VAlign_Center);
    }
    if (const auto material = condition->GetMaterial())
        SetAllMaterials(material);
    if (const auto overlayMaterial = condition->GetOverlayMaterial())
        SetOverlayMaterial(overlayMaterial);
    if (condition->DisablesTick())
        SetActorTickEnabled(false);
}

void ABuilding::RemoveConditions(const UObject* source) {
    for (int i = Conditions.Num()-1; i >= 0; i--) {
        if (Conditions[i]->GetSource() == source)
            RemoveCondition(Conditions[i]);
    }
}

void ABuilding::RemoveCondition(UCondition* condition) {
    const int removed = Conditions.Remove(condition);
    if (removed <= 0)
        return;

    if (condition->Image) {
        auto floatingSymbols = GetComponentByClass<UScalingWidgetComponent>();
        check(floatingSymbols);

        const auto widget = Cast<UConditionListUI>(floatingSymbols->GetWidget());
        check(widget);
        widget->List->RemoveChild(condition->Image);

        if (widget->List->GetChildrenCount() == 0)
            RemoveInstanceComponent(floatingSymbols);
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
    if (condition->GetOverlayMaterial()) {
        UMaterialInterface* newOverlayMat = nullptr;
        for (int i = Conditions.Num()-1; i >= 0; i--) {
            if (Conditions[i]->GetOverlayMaterial()) {
                newOverlayMat = Conditions[i]->GetOverlayMaterial();
                break;
            }
        }
        SetOverlayMaterial(newOverlayMat);
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
