// Fill out your copyright notice in the Description page of Project Settings.

#include "BuildingSelectedUI.h"

#include "The.h"
#include "XD/PlayerControllerX.h"
#include "XD/Encyclopedia/Encyclopedia.h"

UBuildingSelectedUI* UBuildingSelectedUI::Init(ABuilding* actor) {
    const auto encyclopedia = The::Encyclopedia(actor);

    const auto constructionPlan = encyclopedia->GetBuilding(actor->GetClass());
    check(constructionPlan);
    if (!constructionPlan)
        return nullptr;

    Selected = actor;
    Building->Init(constructionPlan);
    Name->SetText(constructionPlan->Name);

    actor->InitSelectedUI(Components);
    for (const auto component : Components)
        component->CreateUI(this);
    
    if (Top->GetChildrenCount() == 0)
        Top->SetVisibility(ESlateVisibility::Collapsed);
    return this;
}

void UBuildingSelectedUI::Tick() {
    if (!Selected)
        return;

    for (const auto component : Components)
        component->Tick(this);
}
